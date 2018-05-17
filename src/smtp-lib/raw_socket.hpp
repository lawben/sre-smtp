#pragma once

#include <string>
#include <vector>
#include "utils.hpp"

using Bytes = std::vector<char>;

class RawSocket{
  public:

#ifdef WIN32
    using SocketType = size_t;
#else
    using SocketType = int;
#endif

    static RawSocket new_socket();

    ~RawSocket();
	RawSocket(const RawSocket&) = delete;
	RawSocket& operator=(const RawSocket&) = delete;
    RawSocket(RawSocket&& other) noexcept;
    RawSocket& operator=(RawSocket&& other) noexcept;

    void bind(int port);
    void listen(int backlog);
    RawSocket accept();
    void connect(std::string& addr, int port);
    Bytes read(size_t size);
    void write(const Bytes& data);
    void write(const std::string& data);

  private:
    explicit RawSocket(SocketType id);
    static std::string get_error();

#ifdef WIN32
    static bool s_initialized;
#endif
    SocketType m_id;
};
