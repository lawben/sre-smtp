#include "mail_receiver.hpp"

#include <iostream>

#include "mail_parser.hpp"

MailReceiver::MailReceiver(Connection connection)
    : m_connection(std::move(connection)), m_parser(MailParser::get_envelop_parser()), m_stop_requested(false) {}

void MailReceiver::run() {
    send_response(get_welcome_response());

    while (no_stop_needed()) {
        const auto bytes = m_connection.read();
        ParserRequest request{bytes};
        if (bytes.empty()) continue;

        auto parser_state = m_parser.accept(request);

        if (parser_state == ParserStatus::INCOMPLETE) continue;
        if (parser_state != ParserStatus::COMPLETE) {
            m_parser.get_command();
            auto response = get_parser_error_response(parser_state);
            send_response(response);
            continue;
        }
        try {
            const auto command = m_parser.get_command();

            if (parser_state == ParserStatus::COMPLETE) {
                auto response = handle_complete_command(command);
                send_response(response);
                continue;
            }
        } catch (const std::runtime_error& e) {
            auto response = get_error_response(e);
            send_response(response);
            continue;
        }
    }
}

void MailReceiver::send_response(const SMTPResponse& response) { m_connection.write(response.get_message()); }

void MailReceiver::stop() { m_stop_requested = true; }

bool MailReceiver::no_stop_needed() { return !m_error_occurred && !m_stop_requested; }

SMTPResponse MailReceiver::handle_complete_command(const SMTPCommand& command) {
    if (m_state_machine.accept(command.type)) {
        //handle_state_transition();
        return handle_accepted_command(command);
    } else {
        return get_not_accepted_response(command.type);
    }
}

SMTPResponse MailReceiver::handle_accepted_command(const SMTPCommand& command) {
    try {
        m_mail_builder.add(command);
    } catch (const std::runtime_error& e) {
        // TODO: log error, return msg to client?
    }

    switch (command.type) {
        case SMTPCommandType::DATA:
            m_parser = MailParser::get_envelop_parser();
            //m_mail_persister.persist(m_mail_builder.build());
            break;
        case SMTPCommandType::DATA_BEGIN:
            m_parser = MailParser::get_content_parser();
            break;
        case SMTPCommandType::HELO:
        case SMTPCommandType::RSET:
        case SMTPCommandType::INVALID:
            m_parser = MailParser::get_envelop_parser();
            break;
    }

    return get_accepted_response(command.type);
}

SMTPResponse MailReceiver::get_welcome_response() const { return {220, "sre - smtp server"}; }
SMTPResponse MailReceiver::get_accepted_response(const SMTPCommandType& type) const {
    switch (type) {
        case SMTPCommandType::DATA_BEGIN:
            return {354, "End Data with <CR><LF>.<CR><LF>"};
        case SMTPCommandType::QUIT:
            return {221, "Bye!"};
        default:
            return {250, "OK"};
    }
}

SMTPResponse MailReceiver::get_not_accepted_response(const SMTPCommandType& type) const {
    return {500, "Invalid command!"};
}

SMTPResponse MailReceiver::get_parser_error_response(const ParserStatus state) const {
    switch (state) {
        case ParserStatus::TO_LONG:
            return {500, "Parser error, received bytes after end token."};
        default:
            return {500, "Unknown parser error."};
    }
}

SMTPResponse MailReceiver::get_error_response(const std::exception& e) const {
    std::string msg("Internal error!");
    msg += e.what();
    return {500, msg};
}