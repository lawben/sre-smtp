#pragma once

#include <string>
#include <memory>
#include <thread>
#include <chrono>

#include "smtp-lib/raw_socket.hpp"

void inline wait_for_network_interaction() {
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
};

bool inline check_return_code(const std::unique_ptr<Connection>& connection, std::string prefix) {
	wait_for_network_interaction();

	const auto bytes = connection->read();
	std::string result(bytes.begin(), bytes.end());
	return result.find(prefix) == 0;
}
