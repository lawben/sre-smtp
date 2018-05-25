#include "mail_receiver.hpp"

#include <iostream>

#include "mail_parser.hpp"

MailReceiver::MailReceiver(Connection connection)
    : m_connection(std::move(connection)), m_parser(MailParser::get_envelop_parser()), m_stop_requested(false) {
    m_state_machine.set_mail_rest_callback(std::bind(&MailReceiver::on_mail_reset, this));
    m_state_machine.set_content_start_callback(std::bind(&MailReceiver::on_content_start, this));
    m_state_machine.set_mail_finish_callback(std::bind(&MailReceiver::on_mail_finished, this));
}

void MailReceiver::run() {
    send_response(get_welcome_response());

    while (no_stop_needed()) {
        ParserRequest request{m_connection.read()};
        if (request.empty()) continue;

        handle_request(request);
    }
}

void MailReceiver::stop() { m_stop_requested = true; }

void MailReceiver::send_response(const SMTPResponse& response) { m_connection.write(response.get_message()); }

bool MailReceiver::no_stop_needed() { return !m_error_occurred && !m_stop_requested; }

void MailReceiver::handle_request(const ParserRequest& request) {
    auto parser_state = m_parser.accept(request);

    if (parser_state != ParserStatus::INCOMPLETE) {
        const auto command = m_parser.get_command();

        SMTPResponse response = get_error_response();
        if (parser_state == ParserStatus::COMPLETE) {
            response = handle_complete_command(command);
        } else {
            response = get_error_response(parser_state);
        }

        if (m_mail_completed) handle_complete_mail();
        if (m_mail_reset) handle_reset_mail();

        send_response(response);
    }
}

SMTPResponse MailReceiver::handle_complete_command(const SMTPCommand& command) {
    if (m_state_machine.accept(command.type)) {
        return handle_accepted_command(command);
    } else {
        return get_error_response(command.type);
    }
}

SMTPResponse MailReceiver::handle_accepted_command(const SMTPCommand& command) {
    try {
        m_mail_builder.add(command);
    } catch (const std::runtime_error& e) {
        // TODO: log error, return msg to client?
    }

    return get_accepted_response(command.type);
}

void MailReceiver::handle_complete_mail() {
    m_mail_completed = false;

    auto mail = m_mail_builder.build();
    // m_mail_persister.persist(mail);
}

void MailReceiver::handle_reset_mail() { 
	m_mail_reset = false;
	m_mail_builder.reset();
	m_parser = MailParser::get_envelop_parser();
}

void MailReceiver::on_mail_finished() {
    m_parser = MailParser::get_envelop_parser();
    m_mail_completed = true;
}

void MailReceiver::on_mail_reset() { m_mail_reset = true; }

void MailReceiver::on_content_start() { m_parser = MailParser::get_content_parser(); }

SMTPResponse MailReceiver::get_welcome_response() { return {220, "sre - smtp server"}; }

SMTPResponse MailReceiver::get_accepted_response(const SMTPCommandType& type) {
    switch (type) {
        case SMTPCommandType::DATA_BEGIN:
            return {354, "End Data with <CR><LF>.<CR><LF>"};
        case SMTPCommandType::QUIT:
            return {221, "Bye!"};
        default:
            return {250, "OK"};
    }
}

SMTPResponse MailReceiver::get_error_response(const SMTPCommandType& type) { return {500, "Invalid command."}; }

SMTPResponse MailReceiver::get_error_response(const ParserStatus state) {
    switch (state) {
        case ParserStatus::TOO_LONG:
            return {500, "Parser error, received bytes after end token."};
        default:
            return {500, "Unknown parser error."};
    }
}

SMTPResponse MailReceiver::get_error_response(const std::exception& e) {
    std::string msg("Internal error: ");
    msg += e.what();
    return {500, msg};
}

SMTPResponse MailReceiver::get_error_response() { return {500, "Internal error."}; }