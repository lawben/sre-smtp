#include "socket.hpp"

#include <memory>

#include "connection.hpp"
#include "mail_receiver.hpp"


Socket::Socket(const uint16_t port) : m_port(port), m_raw_socket(RawSocket::new_socket()) {
    m_raw_socket.bind(m_port);
    m_raw_socket.listen(5);
}

std::unique_ptr<Connection> Socket::accept_connection() {
    // This call blocks until a connection comes in
    auto raw_socket_conn = std::make_unique<RawSocket>(m_raw_socket.accept());
	return std::make_unique<Connection>(std::move(raw_socket_conn));
}
