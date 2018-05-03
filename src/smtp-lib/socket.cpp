#include "socket.hpp"
#include "connection.hpp"

#include <netinet/in.h>
#include <cstdio>
#include <unistd.h>
#include <memory>

Socket::Socket(const uint16_t port) : m_port(port), m_raw_socket{} {
  m_raw_socket.bind(m_port);
  m_raw_socket.listen(10);
}

SMTPHandler Socket::accept_connection() {
  // This call blocks until a connection comes in
  auto raw_socket_conn = std::make_unique<RawSocket>(m_raw_socket.accept());
  auto connection = std::make_unique<Connection>(std::move(raw_socket_conn));
  return SMTPHandler{std::move(connection)};
}
