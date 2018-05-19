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

    bool is_valid() const;

    bool bind(int port);
    bool listen(int backlog);
    RawSocket accept();
    void connect(std::string& addr, int port);
    Bytes read(size_t size);
    bool write(const Bytes& data);
    bool write(const std::string& data);

  private:
    explicit RawSocket(SocketType id);

    static int get_error_id();
    static std::string get_error_string(int error_id);
    static bool is_temporary_error(int error_id);

#ifdef WIN32
    static bool s_initialized;
#endif
    SocketType m_id;
};
