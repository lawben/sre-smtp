#include "mail_receiver.hpp"

#include <iostream>

#include "mail_parser.hpp"

MailReceiver::MailReceiver(std::unique_ptr<Connection> connection) : m_connection(std::move(connection)) {}

void MailReceiver::run() {
    send_response("220 sre-smtp server\r\n");
	std::string response = "";

    while (!m_error_occurred) {
        const auto bytes = m_connection->read();

        ParserRequest request{bytes};
        try {
            const auto smtp_commands = m_parser.accept(request);
            for (auto command : smtp_commands) {
				response = handle_command(command);
            }
        } catch (const std::runtime_error& e) {
			response = "500 : ";
			response += e.what();
        }

		send_response(response);
    }

    return;
}

std::string MailReceiver::handle_command(const SMTPCommand& command) {

	switch (command.type) {
		default:
			return "250 OK";
	}
}

void MailReceiver::send_response(const std::string& msg) {
	m_connection->write(msg);
}