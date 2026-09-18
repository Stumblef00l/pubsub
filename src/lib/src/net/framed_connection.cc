#include "pubsub/net/framed_connection.hpp"

#include <unistd.h>

#include <cerrno>
#include <mutex>
#include <string>
#include <utility>

#include "pubsub/net/reactor.hpp"
#include "pubsub/net/wire_format.hpp"

namespace pubsub {
namespace net {

namespace {

constexpr size_t kReadChunkSize = 4096;

} // namespace

FramedConnection::FramedConnection(
    int fd,
    IReactor* reactor,
    FrameHandler on_frame)
    : fd_(fd),
      reactor_(reactor),
      on_frame_(std::move(on_frame)),
      closed_(false) {}

FramedConnection::~FramedConnection() {
    Close();
}

void
FramedConnection::Start() {
    reactor_->WatchReadable(fd_, [this] { OnReadable(); });
}

void
FramedConnection::Send(
    std::string payload) {
    if (closed_.load())
        return;

    std::string frame = wire_format::EncodeFrame(payload);
    {
        std::lock_guard<std::mutex> lck(outbound_mtx_);
        outbound_buffer_ += frame;
    }

    reactor_->WatchWritable(fd_, [this] { OnWritable(); });
}

void
FramedConnection::Close() {
    bool was_closed = closed_.exchange(true);
    if (was_closed)
        return;

    reactor_->Unwatch(fd_);
    ::close(fd_);
}

bool
FramedConnection::IsClosed() const {
    return closed_.load();
}

void
FramedConnection::OnReadable() {
    if (closed_.load())
        return;

    char buf[kReadChunkSize];
    ssize_t n = ::read(fd_, buf, sizeof(buf));

    if (n > 0) {
        inbound_buffer_.append(buf, static_cast<size_t>(n));
        DrainFrames();
        return;
    }

    if (n == 0) {
        HandleFatalError();
        return;
    }

    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
        return;

    HandleFatalError();
}

void
FramedConnection::DrainFrames() {
    while (true) {
        size_t consumed = 0;
        auto payload = wire_format::TryDecodeFrame(inbound_buffer_, consumed);
        if (!payload.has_value())
            return;

        inbound_buffer_.erase(0, consumed);

        try {
            on_frame_(std::move(*payload));
        } catch (const std::exception&) {
            HandleFatalError();
            return;
        }
    }
}

FramedConnection::FlushResult
FramedConnection::FlushLocked() {
    while (!outbound_buffer_.empty()) {
        ssize_t n = ::write(fd_, outbound_buffer_.data(), outbound_buffer_.size());

        if (n > 0) {
            outbound_buffer_.erase(0, static_cast<size_t>(n));
            continue;
        }

        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return FlushResult::kWouldBlock;

        if (n < 0 && errno == EINTR)
            continue;

        outbound_buffer_.clear();
        return FlushResult::kFatal;
    }

    return FlushResult::kOk;
}

void
FramedConnection::OnWritable() {
    if (closed_.load())
        return;

    FlushResult result;
    {
        std::lock_guard<std::mutex> lck(outbound_mtx_);
        result = FlushLocked();
    }

    switch (result) {
        case FlushResult::kOk:
            reactor_->StopWatchingWritable(fd_);
            break;
        case FlushResult::kWouldBlock:
            break;
        case FlushResult::kFatal:
            HandleFatalError();
            break;
    }
}

void
FramedConnection::HandleFatalError() {
    Close();
}

} // namespace net
} // namespace pubsub
