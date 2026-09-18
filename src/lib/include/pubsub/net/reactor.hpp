#ifndef _PUBSUB_NET_REACTOR_HPP_
#define _PUBSUB_NET_REACTOR_HPP_

#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <unordered_map>

namespace pubsub {
namespace net {

// Provides an interface for a single-threaded, level-triggered readiness
// notifier for file descriptors (an "async IO reactor"). Implementations
// are expected to be safe to call into (Watch*/Unwatch) from any thread
// while Run() is executing on another thread; registered callbacks
// themselves are always invoked from the thread that called Run().
class IReactor {
    public:
        typedef std::function<void()> Callback;

        // Registers (or replaces) the callback invoked whenever fd is
        // readable.
        virtual void
        WatchReadable(
            int fd,
            Callback callback) = 0;

        // Registers (or replaces) the callback invoked whenever fd is
        // writable.
        virtual void
        WatchWritable(
            int fd,
            Callback callback) = 0;

        // Stops watching fd for writability (readability watch, if any, is
        // left untouched). Safe to call even if fd has no writable watch.
        virtual void
        StopWatchingWritable(
            int fd) = 0;

        // Stops watching fd entirely.
        virtual void
        Unwatch(
            int fd) = 0;

        // Runs the event loop on the calling thread until Stop() is called.
        virtual void
        Run() = 0;

        // Causes a concurrently running Run() to return.
        virtual void
        Stop() = 0;

        virtual
        ~IReactor() noexcept {}
};

class EpollReactor: public IReactor {
    public:
        EpollReactor();

        ~EpollReactor() override;

        void
        WatchReadable(
            int fd,
            Callback callback) override;

        void
        WatchWritable(
            int fd,
            Callback callback) override;

        void
        StopWatchingWritable(
            int fd) override;

        void
        Unwatch(
            int fd) override;

        void
        Run() override;

        void
        Stop() override;

    private:
        struct Watch {
            Callback on_readable;
            Callback on_writable;
            uint32_t events = 0;
        };

        void
        UpdateEpoll(
            int fd,
            const Watch& watch,
            bool is_new_watch);

        int epoll_fd_;
        int wake_fd_read_;
        int wake_fd_write_;
        std::mutex mtx_;
        std::unordered_map<int, Watch> watches_;
        std::atomic<bool> running_;
};

} // namespace net
} // namespace pubsub

#endif
