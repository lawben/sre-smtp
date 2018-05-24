#include "mail_receiver.hpp"

#include <iostream>

#include "mail_parser.hpp"

MailReceiver::MailReceiver(Connection connection)
    : m_connection(std::move(connection)), m_parser(MailParser::get_envelop_parser()), m_stop_requested(false) {}

void MailReceiver::run() {
    send_response(get_welcome_response());

    while (no_stop_needed()) {
        SMTPResponse response{0, ""};

        const auto bytes = m_connection.read();
        ParserRequest request{bytes};
        if (bytes.empty()) continue;

        try {
            auto parser_state = m_parser.accept(request);

            if (parser_state != ParserStatus::INCOMPLETE) {
                const auto command = m_parser.get_command();

                if (parser_state == ParserStatus::COMPLETE) {
                    const auto command_accepted = m_state_machine.accept(command.type);

                    if (command_accepted) {
                        m_mail_builder.add(command);

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
                        if (command.type == SMTPCommandType::DATA) {
                        }
                        response = get_accepted_response(command.type);
                    } else {
                        response = get_not_accepted_response(command.type);
                    }
                } else {
                    response = get_parser_error_response(parser_state);
                }
            }
        } catch (const std::runtime_error& e) {
            response = get_error_response(e);
        }
        if (!response.string.empty()) send_response(response);
    }
}

void MailReceiver::send_response(const SMTPResponse& response) { m_connection.write(response.get_message()); }

void MailReceiver::stop() { m_stop_requested = true; }

bool MailReceiver::no_stop_needed() { return !m_error_occurred && !m_stop_requested; }

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