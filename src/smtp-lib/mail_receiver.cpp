#include "mail_receiver.hpp"

#include <iostream>

#include "mail_parser.hpp"

MailReceiver::MailReceiver(Connection connection) : m_connection(std::move(connection)), m_stop_requested(false) {}

void MailReceiver::run() {
    send_response("220 sre-smtp server\r\n");

    while (no_stop_needed()) {
        std::string response = "";
        const auto bytes = m_connection.read();

        ParserRequest request{bytes};
        try {
            const auto smtp_commands = m_parser.accept(request, m_state_machine.current_simplified_state());
            for (const auto& command : smtp_commands) {
                // const auto response = m_state_machine.accept(command.type);
				// bool accepted 
				// if(accepted) {
				//	  m_mail_builder.add(command);
				//	  if(command.type == SMTPCommandType::DATA)
				//	  {
				//	      try {
				//	         auto mail = m_mail_builder.build();
				//	  		  m_mail_persister.persist(mail);
				//	      } catch (const std::runtim_error& e) { }
				//	  } 
				//	  response = get_accepted_response(command.type);
				// } else {
                // response = get_not_accepted_response(command.type);
            }
        } catch (const std::runtime_error& e) {
            // TODO: Do something meaningful
            // send_response(e.what());
            // response = get_error_response(command.type);
		}
        if (!response.empty()) send_response(response);
    }
}

void MailReceiver::send_response(const std::string& msg) { m_connection.write(msg); }

void MailReceiver::stop() { m_stop_requested = true; }

bool MailReceiver::no_stop_needed() { return !m_error_occurred && !m_stop_requested; }
