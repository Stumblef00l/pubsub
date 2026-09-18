#include "pubsub/net/reactor.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "gtest/gtest.h"

namespace {

using namespace std::chrono_literals;

class EpollReactorUnitTest: public ::testing::Test {
    protected:
        void
        SetUp() override {
            ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
            reactor = std::make_unique<pubsub::net::EpollReactor>();
        }

        void
        TearDown() override {
            reactor->Stop();
            if (reactor_thread.joinable())
                reactor_thread.join();
            ::close(fds[0]);
            ::close(fds[1]);
        }

        void
        RunReactorInBackground() {
            reactor_thread = std::thread([this] { reactor->Run(); });
        }

        int fds[2];
        std::unique_ptr<pubsub::net::EpollReactor> reactor;
        std::thread reactor_thread;
};

TEST_F(EpollReactorUnitTest, InvokesCallbackWhenFdBecomesReadable) {
    std::mutex mtx;
    std::condition_variable cv;
    bool fired = false;

    reactor->WatchReadable(fds[0], [&] {
        char buf[16];
        ::read(fds[0], buf, sizeof(buf));
        std::lock_guard<std::mutex> lck(mtx);
        fired = true;
        cv.notify_one();
    });

    RunReactorInBackground();

    char byte = 'x';
    ASSERT_EQ(::write(fds[1], &byte, 1), 1);

    std::unique_lock<std::mutex> lck(mtx);
    ASSERT_TRUE(cv.wait_for(lck, 2s, [&] { return fired; }));
}

TEST_F(EpollReactorUnitTest, InvokesCallbackWhenFdIsWritable) {
    std::mutex mtx;
    std::condition_variable cv;
    bool fired = false;

    // A freshly created socket's send buffer is empty, so it should be
    // reported writable almost immediately.
    reactor->WatchWritable(fds[0], [&] {
        reactor->StopWatchingWritable(fds[0]);
        std::lock_guard<std::mutex> lck(mtx);
        fired = true;
        cv.notify_one();
    });

    RunReactorInBackground();

    std::unique_lock<std::mutex> lck(mtx);
    ASSERT_TRUE(cv.wait_for(lck, 2s, [&] { return fired; }));
}

TEST_F(EpollReactorUnitTest, StopWatchingWritablePreventsFurtherCallbacks) {
    std::mutex mtx;
    std::condition_variable cv;
    int fire_count = 0;

    reactor->WatchWritable(fds[0], [&] {
        reactor->StopWatchingWritable(fds[0]);
        std::lock_guard<std::mutex> lck(mtx);
        fire_count++;
        cv.notify_one();
    });

    RunReactorInBackground();

    {
        std::unique_lock<std::mutex> lck(mtx);
        ASSERT_TRUE(cv.wait_for(lck, 2s, [&] { return fire_count >= 1; }));
    }

    std::this_thread::sleep_for(200ms);

    std::lock_guard<std::mutex> lck(mtx);
    EXPECT_EQ(fire_count, 1);
}

TEST_F(EpollReactorUnitTest, UnwatchStopsFurtherCallbacks) {
    std::mutex mtx;
    std::condition_variable cv;
    int fire_count = 0;

    reactor->WatchReadable(fds[0], [&] {
        char buf[16];
        ::read(fds[0], buf, sizeof(buf));
        std::lock_guard<std::mutex> lck(mtx);
        fire_count++;
        cv.notify_one();
    });

    RunReactorInBackground();

    char byte = 'x';
    ASSERT_EQ(::write(fds[1], &byte, 1), 1);

    {
        std::unique_lock<std::mutex> lck(mtx);
        ASSERT_TRUE(cv.wait_for(lck, 2s, [&] { return fire_count >= 1; }));
    }

    reactor->Unwatch(fds[0]);

    ASSERT_EQ(::write(fds[1], &byte, 1), 1);
    std::this_thread::sleep_for(200ms);

    std::lock_guard<std::mutex> lck(mtx);
    EXPECT_EQ(fire_count, 1);
}

TEST_F(EpollReactorUnitTest, StopCausesRunToReturn) {
    RunReactorInBackground();

    reactor->Stop();

    reactor_thread.join();
    SUCCEED();
}

} // namespace
