#include "connection.hpp"

Connection::Connection(std::unique_ptr<RawSocket> raw_socket) : m_raw_socket(std::move(raw_socket)) {}

std::vector<char> Connection::read() { return m_raw_socket->read(1024); }

void Connection::write(const std::vector<char>& out_bytes) { m_raw_socket->write(out_bytes); }

void Connection::write(const std::string& message) { m_raw_socket->write(Bytes(message.begin(), message.end())); }
