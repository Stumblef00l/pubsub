#include "pubsub/net/socket_utils.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <string>

namespace pubsub {
namespace net {
namespace socket_utils {

namespace {

constexpr int kListenBacklog = 128;

} // namespace

void
SetNonBlocking(
    int fd) {
    int flags = ::fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        throw SocketException("fcntl(F_GETFL) failed: " + std::string(::strerror(errno)));

    if (::fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        throw SocketException("fcntl(F_SETFL) failed: " + std::string(::strerror(errno)));
}

int
CreateListeningSocket(
    const std::string& host,
    uint16_t port) {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    addrinfo* resolved = nullptr;
    std::string port_str = std::to_string(port);
    int rc = ::getaddrinfo(host.empty() ? nullptr : host.c_str(), port_str.c_str(), &hints, &resolved);
    if (rc != 0)
        throw SocketException("getaddrinfo failed: " + std::string(::gai_strerror(rc)));

    int fd = -1;
    for (addrinfo* p = resolved; p != nullptr; p = p->ai_next) {
        fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0)
            continue;

        int reuse = 1;
        ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        if (::bind(fd, p->ai_addr, p->ai_addrlen) == 0)
            break;

        ::close(fd);
        fd = -1;
    }
    ::freeaddrinfo(resolved);

    if (fd < 0)
        throw SocketException("Failed to bind listening socket for " + host + ":" + port_str);

    if (::listen(fd, kListenBacklog) != 0) {
        ::close(fd);
        throw SocketException("listen failed: " + std::string(::strerror(errno)));
    }

    SetNonBlocking(fd);
    return fd;
}

uint16_t
GetLocalPort(
    int fd) {
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    if (::getsockname(fd, reinterpret_cast<sockaddr*>(&addr), &len) != 0)
        throw SocketException("getsockname failed: " + std::string(::strerror(errno)));

    return ntohs(addr.sin_port);
}

int
TryAccept(
    int listen_fd) {
    int fd = ::accept(listen_fd, nullptr, nullptr);
    if (fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            return -1;
        throw SocketException("accept failed: " + std::string(::strerror(errno)));
    }

    SetNonBlocking(fd);
    return fd;
}

int
StartConnect(
    const std::string& host,
    uint16_t port) {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* resolved = nullptr;
    std::string port_str = std::to_string(port);
    int rc = ::getaddrinfo(host.c_str(), port_str.c_str(), &hints, &resolved);
    if (rc != 0)
        throw SocketException("getaddrinfo failed: " + std::string(::gai_strerror(rc)));

    int fd = -1;
    for (addrinfo* p = resolved; p != nullptr; p = p->ai_next) {
        fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0)
            continue;

        SetNonBlocking(fd);

        int rc2 = ::connect(fd, p->ai_addr, p->ai_addrlen);
        if (rc2 == 0 || errno == EINPROGRESS)
            break;

        ::close(fd);
        fd = -1;
    }
    ::freeaddrinfo(resolved);

    if (fd < 0)
        throw SocketException("Failed to initiate connection to " + host + ":" + port_str);

    return fd;
}

void
CheckConnectCompleted(
    int fd) {
    int socket_error = 0;
    socklen_t len = sizeof(socket_error);
    if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &socket_error, &len) != 0)
        throw SocketException("getsockopt(SO_ERROR) failed: " + std::string(::strerror(errno)));

    if (socket_error != 0)
        throw SocketException("Connection failed: " + std::string(::strerror(socket_error)));
}

} // namespace socket_utils
} // namespace net
} // namespace pubsub
