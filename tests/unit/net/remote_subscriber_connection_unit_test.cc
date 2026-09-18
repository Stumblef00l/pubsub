#include "pubsub/net/remote_subscriber_connection.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pubsub/net/framed_connection.hpp"
#include "pubsub/net/reactor.hpp"
#include "pubsub/net/wire_format.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/testing/mock_reactor.hpp"

namespace {

using ::testing::_;
using ::testing::Invoke;
using pubsub::PubsubMessage;
using pubsub::net::FramedConnection;
using pubsub::net::IReactor;
using pubsub::net::RemoteSubscriberConnection;
using pubsub::testing::MockReactor;

class RemoteSubscriberConnectionUnitTest: public ::testing::Test {
    protected:
        void
        SetUp() override {
            ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);

            ON_CALL(reactor, WatchWritable(_, _))
                .WillByDefault(Invoke([this](int, IReactor::Callback cb) {
                    on_writable = cb;
                }));

            auto connection = std::make_unique<FramedConnection>(
                fds[0], &reactor, [](std::string) {});
            subscriber = std::make_unique<RemoteSubscriberConnection>(
                "remote-subscriber-1", std::move(connection));
        }

        void
        TearDown() override {
            subscriber.reset();
            ::close(fds[1]);
        }

        int fds[2];
        MockReactor reactor;
        IReactor::Callback on_writable;
        std::unique_ptr<RemoteSubscriberConnection> subscriber;
};

TEST_F(RemoteSubscriberConnectionUnitTest, UpdateSendsEncodedMessageOverTheConnection) {
    auto message = PubsubMessage{"msg-1", "family-1", "payload-1"};

    subscriber->Update(message);
    ASSERT_TRUE(on_writable);
    on_writable();

    char buf[256];
    ssize_t n = ::read(fds[1], buf, sizeof(buf));
    ASSERT_GT(n, 0);

    size_t consumed = 0;
    auto frame_payload = pubsub::net::wire_format::TryDecodeFrame(std::string(buf, n), consumed);
    ASSERT_TRUE(frame_payload.has_value());
    EXPECT_EQ(pubsub::net::wire_format::DecodeMessage(*frame_payload), message);
}

TEST_F(RemoteSubscriberConnectionUnitTest, CloseMarksConnectionClosed) {
    EXPECT_FALSE(subscriber->IsClosed());

    subscriber->Close();

    EXPECT_TRUE(subscriber->IsClosed());
}

TEST_F(RemoteSubscriberConnectionUnitTest, UpdateAfterCloseIsANoOp) {
    subscriber->Close();

    EXPECT_CALL(reactor, WatchWritable(_, _)).Times(0);
    subscriber->Update(PubsubMessage{"msg-1", "family-1", "payload-1"});
}

TEST_F(RemoteSubscriberConnectionUnitTest, PreservesSubscriberId) {
    EXPECT_EQ(subscriber->GetID(), "remote-subscriber-1");
}

} // namespace
