#pragma once

#include <cstdint>
#include <vector>
#include "connection.hpp"
#include "smtp_handler.hpp"

class Socket : public NonCopyable {
   public:
    explicit Socket(uint16_t port);

    SMTPHandler accept_connection();

   private:
    const int16_t m_port;
    RawSocket m_raw_socket;
};
