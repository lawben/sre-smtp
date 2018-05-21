#pragma once

#include "connection.hpp"

class SocketListener {
  public:
    explicit SocketListener(uint16_t port);

    Connection accept_connection();
    void close();

  private:
    const int16_t m_port;
    RawSocket m_raw_socket;
};
