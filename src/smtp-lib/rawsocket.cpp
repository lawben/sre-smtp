#include "rawsocket.hpp"

#ifdef WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#endif


#ifdef WIN32
bool RawSocket::s_initialized = false;
#endif

RawSocket::RawSocket()
    : m_id(-1)
{
#ifdef WIN32
    if (!s_initialized)
    {
        WSAData wsaData = { 0 };
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
#endif

    m_id = socket(PF_INET, SOCK_STREAM, 0);
}

RawSocket::RawSocket(socketType id)
    : m_id(id)
{

}

RawSocket::~RawSocket()
{
#ifdef WIN32
    closesocket(m_id);
#else
    close(m_id);
#endif
}

void RawSocket::bind(int port)
{
    sockaddr_in serverAddr = { 0 };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    auto error = ::bind(m_id, (sockaddr*)&serverAddr, sizeof(serverAddr));
}

void RawSocket::listen(int backlog)
{
    auto error = ::listen(m_id, backlog);
}

RawSocket RawSocket::accept()
{
    sockaddr_storage serverStorage;
#ifdef WIN32
    auto addr_size = static_cast<int>(sizeof(serverStorage));
#else
    auto addr_size = static_cast<socklen_t>(sizeof(serverStorage));
#endif
    return RawSocket(::accept(m_id, (sockaddr*)&serverStorage, &addr_size));
}

std::vector<char> RawSocket::read(size_t size)
{
    std::vector<char> buffer(size, 0);

#ifdef WIN32
    auto get = recv(m_id, buffer.data(), static_cast<int>(size), 0);
#else
    auto get = ::read(m_id, buffer.data(), size);
#endif
    buffer.resize(get);

    return buffer;
}

void RawSocket::write(const std::vector<char>& data)
{
#ifdef WIN32
    send(m_id, data.data(), static_cast<int>(data.size()), 0);
#else
    ::write(m_id, data.data(), data.size());
#endif

}

void RawSocket::write(const std::string& data)
{
#ifdef WIN32
    send(m_id, data.data(), static_cast<int>(data.size()), 0);
#else
    ::write(m_id, data.data(), data.size());
#endif
}
