#pragma once

#include <vector>

#include "raw_socket.hpp"

class Connection {
  public:
    explicit Connection(RawSocket raw_socket);

    Bytes read();
    void write(const std::vector<char>& bytes);
    void write(const std::string& message);

    bool is_valid();
    void close();

  private:
    RawSocket m_raw_socket;
};
