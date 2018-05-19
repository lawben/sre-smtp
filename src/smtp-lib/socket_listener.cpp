#include "socket_listener.hpp"

#include <memory>
#include <thread>
#include <chrono>

#include "connection.hpp"
#include "mail_receiver.hpp"


SocketListener::SocketListener(const uint16_t port) : m_port(port), m_raw_socket(RawSocket::new_socket()) {
    m_raw_socket.bind(m_port);
    m_raw_socket.listen(5);
}

std::unique_ptr<Connection> SocketListener::accept_connection() {
    // This call blocks until a connection comes in
    auto raw_socket_conn = std::make_unique<RawSocket>(m_raw_socket.accept());
	return std::make_unique<Connection>(std::move(raw_socket_conn));
}
