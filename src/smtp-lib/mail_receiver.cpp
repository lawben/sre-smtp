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
            const auto smtp_commands = m_parser.accept(request, m_state_machine.current_simplified_state());
            for (const auto& command : smtp_commands) {
                const auto response = m_state_machine.accept(command);
                send_response(std::to_string(response.code) + " " + response.string + NEWLINE_TOKEN);
            }
        } catch (const std::runtime_error& e) {
            // TODO: Do something meaningful
            send_response(e.what());
        }
		if(response != "")
			send_response(response);
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

