#pragma once

#include <memory>
#include <vector>
#include "raw_socket.hpp"

class Connection {
  public:
    explicit Connection(std::unique_ptr<RawSocket> raw_socket);

    Bytes read();
    void write(const std::vector<char>& bytes);
    void write(const std::string& message);

	bool is_valid();

    RawSocket* get_socket() { return m_raw_socket.get(); };

  private:
    std::unique_ptr<RawSocket> m_raw_socket;
};
