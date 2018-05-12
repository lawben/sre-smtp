#include "mail_receiver.hpp"

#include "mail_parser.hpp"

MailReceiver::MailReceiver(std::unique_ptr<Connection> connection) : m_connection(std::move(connection)) {}

void MailReceiver::run() {
    std::string greeting = "220 sre-smtp server\r\n";
    m_connection->write(Bytes(greeting.cbegin(), greeting.cend()));

    while (!m_error_occurred) {
        const auto bytes = m_connection->read();

        ParserRequest request{bytes};
        const auto response = m_parser.accept(request);

        // TODO: handle all the things :)
    }

    return;
}
