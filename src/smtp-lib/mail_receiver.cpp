#include "mail_receiver.hpp"

#include <iostream>

#include "mail_parser.hpp"

MailReceiver::MailReceiver(std::unique_ptr<Connection> connection) : m_connection(std::move(connection)) {}

void MailReceiver::run() {
    std::string greeting = "220 sre-smtp server\r\n";
    m_connection->write(Bytes(greeting.cbegin(), greeting.cend()));

    while (!m_error_occurred) {
        const auto bytes = m_connection->read();

        ParserRequest request{bytes};
        try {
            const auto smtp_commands = m_parser.accept(request);
            for (auto token : smtp_commands) {
                std::cout << token.type << " : " << token.data << std::endl;
            }
        } catch (const std::runtime_error& e) {
            std::string msg("500 : ");
            msg += e.what();
            m_connection->write(msg);
        }

        m_connection->write("250");
        // TODO: handle all the things :)
    }

    return;
}
