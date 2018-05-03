#pragma once

#include <vector>
#include "raw_socket.hpp"

class Connection : public NonCopyable {
 public:
  explicit Connection(std::unique_ptr<RawSocket> raw_socket);

  Bytes read();
  void write(const std::vector<char>& bytes);

 private:
  std::unique_ptr<RawSocket> m_raw_socket;
};


