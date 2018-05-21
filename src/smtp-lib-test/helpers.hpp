#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "smtp-lib/raw_socket.hpp"

void inline wait_for_network_interaction() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); };

bool inline check_return_code(Connection& connection, std::string prefix) {
    wait_for_network_interaction();

    const auto bytes = connection.read();
    std::string result(bytes.begin(), bytes.end());
    return result.find(prefix) == 0;
}

bool inline check_return_code(RawSocket& socket, std::string prefix) {
    wait_for_network_interaction();

    const auto bytes = socket.read(1024);
    std::string result(bytes.begin(), bytes.end());
    return result.find(prefix) == 0;
}

void inline clean_up_sockets() { RawSocket::clean_up(); }
