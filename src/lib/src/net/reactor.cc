#include "pubsub/net/reactor.hpp"

#include <sys/epoll.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <mutex>
#include <utility>

namespace pubsub {
namespace net {

namespace {

constexpr int kMaxEpollEvents = 64;

} // namespace

EpollReactor::EpollReactor()
    : running_(true) {
    epoll_fd_ = ::epoll_create1(0);

    int wake_fds[2];
    ::pipe(wake_fds);
    wake_fd_read_ = wake_fds[0];
    wake_fd_write_ = wake_fds[1];

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = wake_fd_read_;
    ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, wake_fd_read_, &ev);
}

EpollReactor::~EpollReactor() {
    Stop();
    ::close(wake_fd_read_);
    ::close(wake_fd_write_);
    ::close(epoll_fd_);
}

void
EpollReactor::WatchReadable(
    int fd,
    Callback callback) {
    std::lock_guard<std::mutex> lck(mtx_);
    auto [it, inserted] = watches_.try_emplace(fd);
    it->second.on_readable = std::move(callback);
    it->second.events |= EPOLLIN;
    UpdateEpoll(fd, it->second, inserted);
}

void
EpollReactor::WatchWritable(
    int fd,
    Callback callback) {
    std::lock_guard<std::mutex> lck(mtx_);
    auto [it, inserted] = watches_.try_emplace(fd);
    it->second.on_writable = std::move(callback);
    it->second.events |= EPOLLOUT;
    UpdateEpoll(fd, it->second, inserted);
}

void
EpollReactor::StopWatchingWritable(
    int fd) {
    std::lock_guard<std::mutex> lck(mtx_);
    auto it = watches_.find(fd);
    if (it == watches_.end())
        return;

    it->second.on_writable = nullptr;
    it->second.events &= ~static_cast<uint32_t>(EPOLLOUT);
    UpdateEpoll(fd, it->second, false);
}

void
EpollReactor::Unwatch(
    int fd) {
    std::lock_guard<std::mutex> lck(mtx_);
    if (watches_.erase(fd) > 0)
        ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
}

void
EpollReactor::UpdateEpoll(
    int fd,
    const Watch& watch,
    bool is_new_watch) {
    epoll_event ev{};
    ev.events = watch.events;
    ev.data.fd = fd;
    ::epoll_ctl(epoll_fd_, is_new_watch ? EPOLL_CTL_ADD : EPOLL_CTL_MOD, fd, &ev);
}

void
EpollReactor::Run() {
    std::array<epoll_event, kMaxEpollEvents> events;

    while (running_) {
        int n = ::epoll_wait(epoll_fd_, events.data(), events.size(), -1);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            break;
        }

        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == wake_fd_read_) {
                char buf[64];
                while (::read(wake_fd_read_, buf, sizeof(buf)) > 0) {}
                continue;
            }

            Callback on_readable;
            Callback on_writable;
            {
                std::lock_guard<std::mutex> lck(mtx_);
                auto it = watches_.find(fd);
                if (it == watches_.end())
                    continue;
                if (events[i].events & (EPOLLIN | EPOLLHUP | EPOLLERR))
                    on_readable = it->second.on_readable;
                if (events[i].events & EPOLLOUT)
                    on_writable = it->second.on_writable;
            }

            if (on_readable)
                on_readable();
            if (on_writable)
                on_writable();
        }
    }
}

void
EpollReactor::Stop() {
    running_ = false;
    char byte = 1;
    ::write(wake_fd_write_, &byte, 1);
}

} // namespace net
} // namespace pubsub
