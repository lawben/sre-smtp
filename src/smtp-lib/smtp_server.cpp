#include <thread>

#include "smtp_server.hpp"
#include "socket.hpp"

SMTPServer::SMTPServer(uint16_t port) : m_port(port), m_is_running(false), m_stop_requested(false) {}

void SMTPServer::run() {
    m_receiverThread = std::thread(&SMTPServer::accept_connections, this);
    m_is_running = true;
}

bool SMTPServer::is_running() { return m_is_running; }

void SMTPServer::stop() {
    m_stop_requested = true;
    m_receiverThread.join();
    m_stop_requested = false;
    m_is_running = false;
}

void SMTPServer::accept_connections() {
    // This can throw if socket bind/listen fails, but the executable should take care of this
    Socket socket{m_port};

    // Start the the handler for a new SMTP request
    auto handle_client = [](SMTPHandler handler) { handler.run(); };

    // This is the server's main loop to accept connections
    while (!m_stop_requested) {
        // This blocks until a new connection comes in.
        auto smtp_handler = socket.accept_connection();

        std::thread thread{handle_client, std::move(smtp_handler)};
        thread.detach();
    }
}
