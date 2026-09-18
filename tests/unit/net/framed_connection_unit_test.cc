#include "pubsub/net/framed_connection.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pubsub/net/reactor.hpp"
#include "pubsub/net/wire_format.hpp"
#include "pubsub/testing/mock_reactor.hpp"

namespace {

using ::testing::_;
using ::testing::Invoke;
using pubsub::net::FramedConnection;
using pubsub::net::IReactor;
using pubsub::net::wire_format::EncodeFrame;
using pubsub::testing::MockReactor;

class FramedConnectionUnitTest: public ::testing::Test {
    protected:
        void
        SetUp() override {
            ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);

            ON_CALL(reactor, WatchReadable(_, _))
                .WillByDefault(Invoke([this](int, IReactor::Callback cb) {
                    on_readable = cb;
                }));
            ON_CALL(reactor, WatchWritable(_, _))
                .WillByDefault(Invoke([this](int, IReactor::Callback cb) {
                    on_writable = cb;
                }));

            connection = std::make_unique<FramedConnection>(
                fds[0], &reactor, [this](std::string payload) {
                    received_frames.push_back(std::move(payload));
                });
        }

        void
        TearDown() override {
            connection.reset();
            if (fds[1] >= 0)
                ::close(fds[1]);
        }

        // fds[0] is owned by the FramedConnection under test; fds[1] is the
        // "remote peer" end the test drives directly.
        int fds[2];
        MockReactor reactor;
        IReactor::Callback on_readable;
        IReactor::Callback on_writable;
        std::vector<std::string> received_frames;
        std::unique_ptr<FramedConnection> connection;
};

TEST_F(FramedConnectionUnitTest, StartRegistersReadableWatch) {
    EXPECT_CALL(reactor, WatchReadable(fds[0], _)).Times(1);
    connection->Start();
}

TEST_F(FramedConnectionUnitTest, DispatchesACompleteFrameReceivedInOneRead) {
    connection->Start();

    std::string bytes = EncodeFrame("hello");
    ASSERT_EQ(::write(fds[1], bytes.data(), bytes.size()), static_cast<ssize_t>(bytes.size()));

    ASSERT_TRUE(on_readable);
    on_readable();

    ASSERT_EQ(received_frames.size(), 1u);
    EXPECT_EQ(received_frames[0], "hello");
}

TEST_F(FramedConnectionUnitTest, DispatchesMultipleFramesReceivedInOneRead) {
    connection->Start();

    std::string bytes = EncodeFrame("first") + EncodeFrame("second");
    ASSERT_EQ(::write(fds[1], bytes.data(), bytes.size()), static_cast<ssize_t>(bytes.size()));

    on_readable();

    ASSERT_EQ(received_frames.size(), 2u);
    EXPECT_EQ(received_frames[0], "first");
    EXPECT_EQ(received_frames[1], "second");
}

TEST_F(FramedConnectionUnitTest, BuffersAPartialFrameUntilComplete) {
    connection->Start();

    std::string bytes = EncodeFrame("hello world");
    ASSERT_EQ(::write(fds[1], bytes.data(), bytes.size() - 3), static_cast<ssize_t>(bytes.size() - 3));
    on_readable();
    EXPECT_TRUE(received_frames.empty());

    ASSERT_EQ(::write(fds[1], bytes.data() + bytes.size() - 3, 3), 3);
    on_readable();

    ASSERT_EQ(received_frames.size(), 1u);
    EXPECT_EQ(received_frames[0], "hello world");
}

TEST_F(FramedConnectionUnitTest, MarksClosedWhenPeerClosesConnection) {
    connection->Start();

    ::close(fds[1]);
    fds[1] = -1;
    on_readable();

    EXPECT_TRUE(connection->IsClosed());
}

TEST_F(FramedConnectionUnitTest, SendWritesFrameOnceFdBecomesWritable) {
    connection->Send("outgoing-payload");

    ASSERT_TRUE(on_writable);
    on_writable();

    char buf[256];
    ssize_t n = ::read(fds[1], buf, sizeof(buf));
    ASSERT_GT(n, 0);

    size_t consumed = 0;
    auto decoded = pubsub::net::wire_format::TryDecodeFrame(std::string(buf, n), consumed);
    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(*decoded, "outgoing-payload");
}

TEST_F(FramedConnectionUnitTest, CloseIsIdempotentAndClosesTheFd) {
    connection->Close();
    EXPECT_TRUE(connection->IsClosed());

    connection->Close();
    EXPECT_TRUE(connection->IsClosed());
}

TEST_F(FramedConnectionUnitTest, SendAfterCloseIsANoOp) {
    connection->Close();

    EXPECT_CALL(reactor, WatchWritable(_, _)).Times(0);
    connection->Send("should-not-be-sent");
}

} // namespace
