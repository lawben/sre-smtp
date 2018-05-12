#include <thread>

#include "smtp_server.hpp"
#include "socket.hpp"

SMTPServer::SMTPServer(uint16_t port) :
	m_port(port)
{
}

void SMTPServer::run() {
	m_receiverThread = std::thread(accept_connections, m_port);
	m_is_running = true;
}

bool SMTPServer::is_running() {
	return m_is_running;
}

void SMTPServer::stop() {
	m_receiverThread.join();
	m_is_running = false;
}

void SMTPServer::accept_connections(uint16_t port) {
	// This can throw if socket bind/listen fails, but the executable should take care of this
	Socket socket{ port };

	// Start the the handler for a new SMTP request
	auto handle_client = [](SMTPHandler handler) { handler.run(); };

	// This is the server's main loop to accept connections
	while (true) {
		// This blocks until a new connection comes in.
		auto smtp_handler = socket.accept_connection();

		std::thread thread{ handle_client, std::move(smtp_handler) };
		thread.detach();
	}
}
