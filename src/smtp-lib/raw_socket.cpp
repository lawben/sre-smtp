#include "raw_socket.hpp"

#ifdef WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#endif

#include <cerrno>
#include <stdexcept>

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
}  // namespace

RawSocket RawSocket::new_socket() {
#ifdef WIN32
    if (!s_initialized) {
        WSAData wsaData = {0};
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        s_initialized = true;
    }
#endif

    const SocketType descriptor_id = socket(PF_INET, SOCK_STREAM, 0);
    if (descriptor_id == INVALID_SOCKET_ID) {
        throw std::runtime_error(get_error_string(get_error_id()));
    }

    return RawSocket{descriptor_id};
}

void RawSocket::clean_up() {
#ifdef WIN32
    WSACleanup();
    s_initialized = false;
#endif
};

RawSocket RawSocket::new_socket(uint16_t port) {
    auto socket = new_socket();
    socket.bind(port);
    return socket;
}
RawSocket::RawSocket(SocketType id) : m_id(id) {
    if (is_valid()) {
#ifdef WIN32
        u_long mode = 1;
        auto error = ioctlsocket(m_id, FIONBIO, &mode);
#else
        int flags = fcntl(m_id, F_GETFL, 0);
        auto error = fcntl(m_id, F_SETFL, flags | O_NONBLOCK);
#endif

        if (error == ERROR_CONSTANT) {
            throw std::runtime_error(get_error_string(get_error_id()));
        }
    }
}

RawSocket::~RawSocket() { close(); }

bool RawSocket::is_valid() const { return m_id != INVALID_SOCKET_ID; }

bool RawSocket::bind(int port) {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    const auto error = ::bind(m_id, (sockaddr*)&server_addr, sizeof(server_addr));

    if (error == ERROR_CONSTANT) {
        auto error_id = get_error_id();
        if (is_temporary_error(error_id)) {
            return false;
        }
        throw std::runtime_error("Bind socket failed!");
        // throw std::runtime_error(get_error_string(error_id)); TODO: get error string is produces a sigseg.
    }
    return true;
}

bool RawSocket::listen(int backlog) {
    auto error = ::listen(m_id, backlog);

    if (error == ERROR_CONSTANT) {
        auto error_id = get_error_id();
        if (is_temporary_error(error_id)) {
            return false;
        }
        throw std::runtime_error(get_error_string(error_id));
    }
    return true;
}

RawSocket RawSocket::accept() {
    sockaddr_storage server_storage{};
#ifdef WIN32
    auto addr_size = static_cast<int>(sizeof(server_storage));
#else
    auto addr_size = static_cast<socklen_t>(sizeof(server_storage));
#endif
    const auto id = ::accept(m_id, (sockaddr*)&server_storage, &addr_size);
    return RawSocket{id};
}

void RawSocket::connect(std::string& addr, int port) {
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
#ifdef WIN32
    inet_pton(AF_INET, addr.c_str(), &clientService.sin_addr.s_addr);
#else
    clientService.sin_addr.s_addr = inet_addr(addr.c_str());
#endif
    clientService.sin_port = htons(static_cast<u_short>(port));

    auto error = ::connect(m_id, (sockaddr*)&clientService, sizeof(clientService));

    if (error == ERROR_CONSTANT) {
        auto error_id = get_error_id();
        if (!is_temporary_error(error_id)) {
            throw std::runtime_error(get_error_string(error_id));
        }
    }
}
std::vector<char> RawSocket::read(size_t size) {
    Bytes buffer(size, 0);

#ifdef WIN32
    auto get = recv(m_id, buffer.data(), static_cast<int>(size), 0);
#else
    auto get = ::read(m_id, buffer.data(), size);
#endif

    if (get == ERROR_CONSTANT) {
        auto error_id = get_error_id();
        if (is_temporary_error(error_id)) {
            get = 0;
        } else {
            throw std::runtime_error(get_error_string(error_id));
        }
    }

    buffer.resize(get);
    return buffer;
}

bool RawSocket::write(const Bytes& data) {
#ifdef WIN32
    const auto error = send(m_id, data.data(), static_cast<int>(data.size()), 0);
#else
    const auto error = ::write(m_id, data.data(), data.size());
#endif

    if (error == ERROR_CONSTANT) {
        auto error_id = get_error_id();
        if (is_temporary_error(error_id)) {
            return false;
        }
        throw std::runtime_error(get_error_string(error_id));
    }
    return true;
}

bool RawSocket::write(const std::string& data) {
// std::string::data() returns the string + '\0' which is not included in std::string::size()
// TODO: check if this poses a problem
#ifdef WIN32
    const auto error = send(m_id, data.data(), static_cast<int>(data.size()), 0);
#else
    const auto error = ::write(m_id, data.data(), data.size());
#endif

    if (error == ERROR_CONSTANT) {
        auto error_id = get_error_id();
        if (is_temporary_error(error_id)) {
            return false;
        }
        throw std::runtime_error(get_error_string(error_id));
    }
    return true;
}

void RawSocket::close() {
#ifdef WIN32
    shutdown(m_id, SD_BOTH);
    closesocket(m_id);
#else
    ::close(m_id);
#endif
}

int RawSocket::get_error_id() {
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

bool RawSocket::is_temporary_error(int error_id) {
#ifdef WIN32
    return error_id == WSAEWOULDBLOCK;
#else
    return error_id == EAGAIN || error_id == EWOULDBLOCK;
#endif
}

std::string RawSocket::get_error_string(int error_id) {
#ifdef WIN32
    char* error_buffer = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0,
                   error_id, GetSystemDefaultLangID(), (LPSTR)&error_buffer, 0, nullptr);
    return error_buffer;
#else
    return std::strerror(error_id);
#endif
}

RawSocket::RawSocket(RawSocket&& other) noexcept : m_id(other.m_id) { other.m_id = INVALID_SOCKET_ID; }

RawSocket& RawSocket::operator=(RawSocket&& other) noexcept {
    if (this != &other) {
        m_id = other.m_id;
        other.m_id = INVALID_SOCKET_ID;
    }

    return *this;
}
