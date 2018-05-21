#include "socket_listener.hpp"

#include "connection.hpp"

SocketListener::SocketListener(const uint16_t port) : m_port(port), m_raw_socket(RawSocket::new_socket()) {
    m_raw_socket.bind(m_port);
    m_raw_socket.listen(5);
}

Connection SocketListener::accept_connection() { return Connection(m_raw_socket.accept()); }

void SocketListener::close() { m_raw_socket.close(); }