#include <thread>

#include "smtp_server.hpp"
#include "socket.hpp"

SMTPServer::SMTPServer(uint16_t port) : m_port(port), m_is_running(false), m_stop_requested(false) {}

void SMTPServer::run() {
	m_stop_requested = false;
    m_is_running = true;

	accept_connections();

	stop_mail_receivers();
}

bool SMTPServer::is_running() { return m_is_running; }

void SMTPServer::stop() {
    m_stop_requested = true;
    m_is_running = false;
}

void SMTPServer::accept_connections() {
    while (!m_stop_requested) {
		wait_incoming_connection();
		start_worker_for_last_mail_receiver();
    }
}

void SMTPServer::wait_incoming_connection() {
	Socket socket{ m_port };
	auto connection = std::move(socket.accept_connection());
	//MailReceiver gg(std::move(connection));
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
	for (auto& mail_receiver : m_mail_receivers)
	{
		mail_receiver.stop();
	}
}

void SMTPServer::join_worker_threads() {
	for (auto& worker : m_workers)
	{
		worker.join();
	}
}
