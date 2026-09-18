#ifndef _PUBSUB_TESTING_MOCK_REACTOR_HPP_
#define _PUBSUB_TESTING_MOCK_REACTOR_HPP_

#include "pubsub/net/reactor.hpp"

#include "gmock/gmock.h"

namespace pubsub {
namespace testing {

class MockReactor: public net::IReactor {
    public:
        MOCK_METHOD(void, WatchReadable, (int fd, Callback callback), (override));
        MOCK_METHOD(void, WatchWritable, (int fd, Callback callback), (override));
        MOCK_METHOD(void, StopWatchingWritable, (int fd), (override));
        MOCK_METHOD(void, Unwatch, (int fd), (override));
        MOCK_METHOD(void, Run, (), (override));
        MOCK_METHOD(void, Stop, (), (override));
};

} // namespace testing
} // namespace pubsub

#endif
