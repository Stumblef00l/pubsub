#ifndef _PUBSUB_NET_FRAMED_CONNECTION_HPP_
#define _PUBSUB_NET_FRAMED_CONNECTION_HPP_

#include <atomic>
#include <functional>
#include <mutex>
#include <string>

#include "pubsub/net/reactor.hpp"

namespace pubsub {
namespace net {

// Provides length-prefixed, asynchronous, non-blocking framing over a
// connected TCP socket fd, driven by an IReactor. This is the single place
// in the pubsub::net module that performs actual socket reads/writes.
//
// Threading model: reads and writes against the underlying fd only ever
// happen on the thread running the owning IReactor's Run() loop. Send() may
// safely be called from any thread; it only ever buffers the payload and
// asks the reactor to notify this connection when the fd is next writable,
// which is where the actual write() call happens. This avoids a fd being
// close()d on one thread while another thread is in the middle of a
// read()/write() on it.
class FramedConnection {
    public:
        typedef std::function<void(std::string)> FrameHandler;

        // Takes ownership of fd (an already-connected, non-blocking socket).
        // on_frame is invoked (on the reactor's thread) with the payload of
        // each complete frame received.
        FramedConnection(
            int fd,
            IReactor* reactor,
            FrameHandler on_frame);

        FramedConnection(const FramedConnection&) = delete;
        FramedConnection& operator= (const FramedConnection&) = delete;

        ~FramedConnection();

        // Begins watching the fd for incoming frames. Must be called before
        // any data will be delivered to on_frame.
        void
        Start();

        // Encodes payload as a frame and asynchronously writes it to the
        // socket. Safe to call from any thread; a no-op if the connection is
        // already closed.
        void
        Send(
            std::string payload);

        // Idempotent. Stops watching the fd and closes it.
        void
        Close();

        bool
        IsClosed() const;

        int
        fd() const {
            return fd_;
        }

    private:
        enum class FlushResult {
            kOk,
            kWouldBlock,
            kFatal
        };

        void
        OnReadable();

        void
        OnWritable();

        void
        DrainFrames();

        FlushResult
        FlushLocked();

        void
        HandleFatalError();

        int fd_;
        IReactor* reactor_;
        FrameHandler on_frame_;
        std::atomic<bool> closed_;

        // Only ever touched from the reactor thread (inside OnReadable).
        std::string inbound_buffer_;

        std::mutex outbound_mtx_;
        std::string outbound_buffer_;
};

} // namespace net
} // namespace pubsub

#endif
