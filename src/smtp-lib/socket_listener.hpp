#pragma once

#include <cstdint>
#include <vector>

#include <memory>

#include "connection.hpp"
#include "mail_receiver.hpp"

class SocketListener {
  public:
    explicit SocketListener(uint16_t port);

    std::unique_ptr<Connection> accept_connection();

  private:
    const int16_t m_port;
    RawSocket m_raw_socket;
};
