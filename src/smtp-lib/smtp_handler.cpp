#include "smtp_handler.hpp"

#include "mail_parser.hpp"

SMTPHandler::SMTPHandler(std::unique_ptr<Connection> connection) : m_connection(std::move(connection)) {}

void SMTPHandler::run() {
  while(!m_error_occurred) {
    const auto bytes = m_connection->read();

    ParserRequest request{bytes};
    const auto response = m_parser.accept(request);

    if (response.is_error) {
      m_connection->write(response.to_bytes());
      // TODO: error handling etc.
      m_error_occurred = true;
      return;
    }

    m_connection->write(response.to_bytes());

    // TODO: handle all the things :)
  }
}
