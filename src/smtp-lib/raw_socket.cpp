#include "raw_socket.hpp"

#ifdef WIN32
#include <WinSock2.h>
#else
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <stdexcept>
#include <cerrno>

#ifdef WIN32
bool RawSocket::s_initialized = false;
#endif

namespace {
#ifdef WIN32
    static const RawSocket::SocketType INVALID_SOCKET_ID = INVALID_SOCKET;
    static const auto ERROR_CONSTANT = SOCKET_ERROR;
#else
    static RawSocket::SocketType INVALID_SOCKET_ID = -1;
    static const auto ERROR_CONSTANT = -1;
#endif
}

RawSocket::RawSocket()
    : m_id(INVALID_SOCKET_ID)
{
#ifdef WIN32
    if (!s_initialized)
    {
        WSAData wsaData = { 0 };
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
#endif

    m_id = socket(PF_INET, SOCK_STREAM, 0);

    if (m_id == INVALID_SOCKET_ID)
    {
        throw std::runtime_error(get_error());
    }
}

RawSocket::RawSocket(SocketType id)
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
    sockaddr_in server_addr = { 0 };
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    auto error = ::bind(m_id, (sockaddr*)&server_addr, sizeof(server_addr));
    if (error == ERROR_CONSTANT)
    {
        throw std::runtime_error(get_error());
    }
}

void RawSocket::listen(int backlog)
{
    auto error = ::listen(m_id, backlog);
    if (error == ERROR_CONSTANT)
    {
        throw std::runtime_error(get_error());
    }
}

RawSocket RawSocket::accept()
{
    sockaddr_storage server_storage;
#ifdef WIN32
    auto addr_size = static_cast<int>(sizeof(server_storage));
#else
    auto addr_size = static_cast<socklen_t>(sizeof(server_storage));
#endif
    return RawSocket(::accept(m_id, (sockaddr*)&server_storage, &addr_size));
}

std::vector<char> RawSocket::read(size_t size)
{
    Bytes buffer(size, 0);

#ifdef WIN32
    auto get = recv(m_id, buffer.data(), static_cast<int>(size), 0);
#else
    auto get = ::read(m_id, buffer.data(), size);
#endif
    if (get == ERROR_CONSTANT)
    {
        throw std::runtime_error(get_error());
    }

    buffer.resize(get);
    return buffer;
}

void RawSocket::write(const std::vector<char>& data)
{
#ifdef WIN32
    auto error = send(m_id, data.data(), static_cast<int>(data.size()), 0);
#else
    auto error = ::write(m_id, data.data(), data.size());
#endif

    if (error == ERROR_CONSTANT)
    {
        throw std::runtime_error(get_error());
    }
}

void RawSocket::write(const std::string& data)
{
    // std::string::data() returns the string + '\0' which is not included in std::string::size()
    // TODO: check if this poses a problem
#ifdef WIN32
    auto error = send(m_id, data.data(), static_cast<int>(data.size()), 0);
#else
    auto error = ::write(m_id, data.data(), data.size());
#endif

    if (error == ERROR_CONSTANT)
    {
        throw std::runtime_error(get_error());
    }
}

std::string RawSocket::get_error()
{
#ifdef WIN32
    int error = WSAGetLastError();
    char* error_buffer = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, error, GetSystemDefaultLangID(), (LPSTR)&errorBuffer, 0, nullptr);
    return error_buffer;
#else
    return std::strerror(errno);
#endif
}
