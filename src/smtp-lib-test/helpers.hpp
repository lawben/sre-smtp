#pragma once

#include <string>
#include <memory>
#include <thread>
#include <chrono>

#include "smtp-lib/raw_socket.hpp"

class TestClient {
public:
	TestClient(std::string host_addr, uint16_t host_port, uint16_t port = 5900) {
		m_raw_socket = std::make_unique<RawSocket>(RawSocket::new_socket());
		m_raw_socket->bind(port);
		m_raw_socket->connect(std::string(host_addr), host_port);
	}

	void write(const std::string& message) { m_raw_socket->write(Bytes(message.begin(), message.end())); }

private:
	std::unique_ptr<RawSocket> m_raw_socket;
};

void inline wait_for_network_interaction() {
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
};


bool inline check_return_code(const std::unique_ptr<Connection>& connection, std::string prefix) {
	wait_for_network_interaction();

	const auto bytes = connection->read();
	std::string result(bytes.begin(), bytes.end());
	return result.find(prefix) == 0;
}
