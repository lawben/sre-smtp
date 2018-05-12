#include "raw_socket.hpp"

#ifdef WIN32
#include <WinSock2.h>
#else
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
        throw std::runtime_error(get_error());
    }

    return RawSocket{descriptor_id};
}

RawSocket::RawSocket(SocketType id) : m_id(id) {}

RawSocket::~RawSocket() {
    if (m_id != INVALID_SOCKET_ID) {
#ifdef WIN32
        closesocket(m_id);
#else
        close(m_id);
#endif
    }
}

void RawSocket::bind(int port) {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    const auto error = ::bind(m_id, (sockaddr*)&server_addr, sizeof(server_addr));
    if (error == ERROR_CONSTANT) {
        throw std::runtime_error(get_error());
    }
}

void RawSocket::listen(int backlog) {
    auto error = ::listen(m_id, backlog);
    if (error == ERROR_CONSTANT) {
        throw std::runtime_error(get_error());
    }
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
	clientService.sin_addr.s_addr = inet_addr(addr.c_str());
	clientService.sin_port = htons(static_cast<u_short>(port));

	auto iResult = ::connect(m_id, (sockaddr *)& clientService, sizeof(clientService));

#ifdef WIN32
	if (iResult == SOCKET_ERROR) {
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		iResult = closesocket(m_id);
		if (iResult == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
	}
#else 
	if (0 > iResult)
		throw std::runtime_error(get_error());
#endif 

}
std::vector<char> RawSocket::read(size_t size) {
    Bytes buffer(size, 0);

#ifdef WIN32
    const auto get = recv(m_id, buffer.data(), static_cast<int>(size), 0);
#else
    const auto get = ::read(m_id, buffer.data(), size);
#endif
    if (get == ERROR_CONSTANT) {
        throw std::runtime_error(get_error());
    }

    buffer.resize(get);
    return buffer;
}

void RawSocket::write(const Bytes& data) {
#ifdef WIN32
    const auto error = send(m_id, data.data(), static_cast<int>(data.size()), 0);
#else
    const auto error = ::write(m_id, data.data(), data.size());
#endif

    if (error == ERROR_CONSTANT) {
        throw std::runtime_error(get_error());
    }
}

void RawSocket::write(const std::string& data) {
    // std::string::data() returns the string + '\0' which is not included in std::string::size()
    // TODO: check if this poses a problem
#ifdef WIN32
    const auto error = send(m_id, data.data(), static_cast<int>(data.size()), 0);
#else
    const auto error = ::write(m_id, data.data(), data.size());
#endif

    if (error == ERROR_CONSTANT) {
        throw std::runtime_error(get_error());
    }
}

std::string RawSocket::get_error() {
#ifdef WIN32
    int error = WSAGetLastError();
    char* error_buffer = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0,
                   error, GetSystemDefaultLangID(), (LPSTR)&error_buffer, 0, nullptr);
    return error_buffer;
#else
    return std::strerror(errno);
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
