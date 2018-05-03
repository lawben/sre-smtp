#pragma once

#include <string>
#include <vector>


class RawSocket {
public:
    RawSocket();
    ~RawSocket();
    RawSocket(const RawSocket&) = delete;
    RawSocket(RawSocket&&) = default;

    RawSocket& operator=(const RawSocket&) = delete;
    RawSocket& operator=(RawSocket&&) = default;

    void bind(int port);
    void listen(int backlog);
    RawSocket accept();
    std::vector<char> read(size_t size);
    void write(const std::vector<char>& data);
    void write(const std::string& data);

private:
#ifdef WIN32
    using socketType = size_t;
#else
    using socketType = int;
#endif

    explicit RawSocket(socketType id);

#ifdef WIN32
    static bool s_initialized;
#endif
    socketType m_id;
};
