#include "mail_receiver.hpp"

#include <iostream>

#include "mail_parser.hpp"

MailReceiver::MailReceiver(std::unique_ptr<Connection> connection) : m_connection(std::move(connection)) {}

void MailReceiver::run() {
    send_response("220 sre-smtp server\r\n");

    while (!m_error_occurred && m_state_machine.current_state() != SMTPState::FINISHED) {
        const auto bytes = m_connection->read();

        ParserRequest request{bytes};
        try {
            const auto smtp_commands = m_parser.accept(request, m_state_machine.current_simplified_state());
            for (auto command : smtp_commands) {
                const auto response = m_state_machine.accept(command);
                send_response(std::to_string(response.code) + " " + response.string);
            }
        } catch (const std::runtime_error& e) {
            // TODO: Do something meaningful
            send_response(e.what());
        }
    }

    return;
}

void MailReceiver::send_response(const std::string& msg) {
    m_connection->write(msg);
}