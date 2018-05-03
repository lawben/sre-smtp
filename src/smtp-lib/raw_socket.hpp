#pragma once

#include <string>
#include <vector>
#include "utils.hpp"

using Bytes = std::vector<char>;

class RawSocket : public NonCopyable {
public:
#ifdef WIN32
    using SocketType = size_t;
#else
    using SocketType = int;
#endif

    RawSocket();
    ~RawSocket();
    RawSocket(RawSocket&&) = default;
    RawSocket& operator=(RawSocket&&) = default;

    void bind(int port);
    void listen(int backlog);
    RawSocket accept();
    Bytes read(size_t size);
    void write(const Bytes& data);
    void write(const std::string& data);

private:
    explicit RawSocket(SocketType id);
    std::string get_error();

#ifdef WIN32
    static bool s_initialized;
#endif
    SocketType m_id;
};
