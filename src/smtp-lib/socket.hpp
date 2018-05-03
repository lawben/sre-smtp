#pragma once

#include <vector>
#include <cstdint>
#include "connection.hpp"
#include "smtp_handler.hpp"

class Socket : public NonCopyable {
 public:
  explicit Socket(uint16_t port = 5555);

  SMTPHandler accept_connection();

 private:
  RawSocket m_raw_socket;
  const int16_t m_port;
};


