#ifndef _PUBSUB_NET_SOCKET_UTILS_HPP_
#define _PUBSUB_NET_SOCKET_UTILS_HPP_

#include <cstdint>
#include <exception>
#include <string>
#include <string_view>
#include <utility>

namespace pubsub {
namespace net {
namespace socket_utils {

// Creates a non-blocking TCP socket, binds it to host:port and starts
// listening. If port is 0, the OS assigns an ephemeral port (use
// GetLocalPort to discover it). Throws SocketException on failure.
int
CreateListeningSocket(
    const std::string& host,
    uint16_t port);

// Returns the local port a bound socket (e.g. one returned by
// CreateListeningSocket) is using.
uint16_t
GetLocalPort(
    int fd);

// Accepts one pending connection on listen_fd, returning a new non-blocking
// fd for it. Returns -1 if no connection is currently pending. Throws
// SocketException on unexpected failure.
int
TryAccept(
    int listen_fd);

// Creates a non-blocking socket and begins connecting it to host:port. The
// connection is typically still in progress (EINPROGRESS) when this
// returns; callers should watch the fd for writability and then call
// CheckConnectCompleted. Throws SocketException if the connection could not
// even be initiated.
int
StartConnect(
    const std::string& host,
    uint16_t port);

// Called once fd (returned by StartConnect) becomes writable, to check
// whether the underlying connection attempt actually succeeded. Throws
// SocketException if it failed.
void
CheckConnectCompleted(
    int fd);

void
SetNonBlocking(
    int fd);

class SocketException: public std::exception {
    public:
        explicit
        SocketException(
            std::string message)
            : message_(std::move(message)) {}

        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return message_.data();
        };

    private:
        std::string message_;
};

} // namespace socket_utils
} // namespace net
} // namespace pubsub

#endif
