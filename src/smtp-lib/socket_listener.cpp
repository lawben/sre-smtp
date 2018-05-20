#include "socket_listener.hpp"

#include <memory>

#include "connection.hpp"

SocketListener::SocketListener(const uint16_t port) : m_port(port), m_raw_socket(RawSocket::new_socket()) {
    m_raw_socket.bind(m_port);
    m_raw_socket.listen(5);
}

std::unique_ptr<Connection> SocketListener::accept_connection() {
	return std::make_unique<Connection>(m_raw_socket.accept());
}
