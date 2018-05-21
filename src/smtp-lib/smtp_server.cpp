#include <thread>

#include "smtp_server.hpp"
#include "socket_listener.hpp"

SMTPServer::SMTPServer(uint16_t port) : m_socket_listener(port), m_is_running(false), m_stop_requested(false) {}

void SMTPServer::run() {
    m_stop_requested = false;
    m_is_running = true;

    accept_connections();

    stop_mail_receivers();

    RawSocket::clean_up();
}

bool SMTPServer::is_running() { return m_is_running; }

void SMTPServer::stop() {
    m_stop_requested = true;
    m_is_running = false;
}

void SMTPServer::accept_connections() {
    while (!m_stop_requested) {
        auto connection = accept_connection();
        if (connection.is_valid()) {
            add_new_mail_receiver_for(std::move(connection));
            start_worker_for_last_mail_receiver();
        }
    }
}

Connection SMTPServer::accept_connection() { return Connection(m_socket_listener.accept_connection()); }

void SMTPServer::add_new_mail_receiver_for(Connection connection) {
    m_mail_receivers.emplace_back(std::move(connection));
}

void SMTPServer::start_worker_for_last_mail_receiver() {
    m_workers.emplace_back(&MailReceiver::run, &m_mail_receivers.back());
}

void SMTPServer::stop_mail_receivers() {
    request_stop_on_mail_receivers();
    join_worker_threads();
}

void SMTPServer::request_stop_on_mail_receivers() {
    for (auto& mail_receiver : m_mail_receivers) {
        mail_receiver.stop();
    }
}

void SMTPServer::join_worker_threads() {
    for (auto& worker : m_workers) {
        worker.join();
    }
}
