#pragma once

#include <cstdint>
#include <vector>

#include "connection.hpp"
#include "mail_receiver.hpp"

class Socket {
  public:
    explicit Socket(uint16_t port);

    std::unique_ptr<Connection> accept_connection();

  private:
    const int16_t m_port;
    RawSocket m_raw_socket;
};
