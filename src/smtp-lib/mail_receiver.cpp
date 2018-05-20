#include "mail_receiver.hpp"

#include <iostream>

#include "mail_parser.hpp"

MailReceiver::MailReceiver(Connection connection) 
	: m_connection(std::move(connection)),
	m_stop_requested(false){}

void MailReceiver::run() {
    send_response("220 sre-smtp server\r\n");

    while (NoStopNeeded()) {
		std::string response = "";
		const auto bytes = m_connection.read();

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
		if(response != "")
			send_response(response);
    }
}

std::string MailReceiver::handle_command(const SMTPCommand& command) {

	switch (command.type) {
	case SMTPCommandType::HELO:
		return "250 ---";
	case SMTPCommandType::MAIL:
		return "250 OK";
	case SMTPCommandType::RCPT:
		return "250 OK";
	case SMTPCommandType::DATA:
		return "354";
	case SMTPCommandType::DATA_BODY:
		return "250 OK";
	case SMTPCommandType::QUIT:
		return "221";
	default:
		return "500 Bad Syntax";
	}
}

void MailReceiver::send_response(const std::string& msg) {
	m_connection.write(msg);
}

void MailReceiver::stop() {
	m_stop_requested = true;
}

bool MailReceiver::NoStopNeeded() {
	return !m_error_occurred && !m_stop_requested;
}
