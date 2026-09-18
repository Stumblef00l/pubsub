#include "pubsub/net/wire_format.hpp"

#include <string>

#include "gtest/gtest.h"
#include "pubsub/structs.hpp"

namespace {

using pubsub::PubsubMessage;
using pubsub::net::wire_format::AppendLengthPrefixed;
using pubsub::net::wire_format::DecodeMessage;
using pubsub::net::wire_format::DecodeSubscriptionRequest;
using pubsub::net::wire_format::EncodeFrame;
using pubsub::net::wire_format::EncodeMessage;
using pubsub::net::wire_format::EncodeSubscriptionRequest;
using pubsub::net::wire_format::MalformedFrameException;
using pubsub::net::wire_format::ReadLengthPrefixed;
using pubsub::net::wire_format::SubscriptionRequest;
using pubsub::net::wire_format::TryDecodeFrame;

TEST(LengthPrefixedUnitTest, RoundTripsSingleValue) {
    std::string buffer;
    AppendLengthPrefixed(buffer, "hello");

    size_t offset = 0;
    EXPECT_EQ(ReadLengthPrefixed(buffer, offset), "hello");
    EXPECT_EQ(offset, buffer.size());
}

TEST(LengthPrefixedUnitTest, RoundTripsEmptyValue) {
    std::string buffer;
    AppendLengthPrefixed(buffer, "");

    size_t offset = 0;
    EXPECT_EQ(ReadLengthPrefixed(buffer, offset), "");
    EXPECT_EQ(offset, buffer.size());
}

TEST(LengthPrefixedUnitTest, RoundTripsMultipleValuesInSequence) {
    std::string buffer;
    AppendLengthPrefixed(buffer, "first");
    AppendLengthPrefixed(buffer, "second-value");

    size_t offset = 0;
    EXPECT_EQ(ReadLengthPrefixed(buffer, offset), "first");
    EXPECT_EQ(ReadLengthPrefixed(buffer, offset), "second-value");
    EXPECT_EQ(offset, buffer.size());
}

TEST(LengthPrefixedUnitTest, ThrowsOnTruncatedLengthPrefix) {
    std::string buffer = "ab"; // fewer than 4 bytes
    size_t offset = 0;
    EXPECT_THROW(ReadLengthPrefixed(buffer, offset), MalformedFrameException);
}

TEST(LengthPrefixedUnitTest, ThrowsOnTruncatedPayload) {
    std::string buffer;
    AppendLengthPrefixed(buffer, "hello");
    buffer.resize(buffer.size() - 2); // truncate the payload

    size_t offset = 0;
    EXPECT_THROW(ReadLengthPrefixed(buffer, offset), MalformedFrameException);
}

TEST(FrameUnitTest, TryDecodeReturnsNulloptWhenBufferEmpty) {
    std::string buffer;
    size_t consumed = 0;
    EXPECT_FALSE(TryDecodeFrame(buffer, consumed).has_value());
}

TEST(FrameUnitTest, TryDecodeReturnsNulloptWhenLengthPrefixIncomplete) {
    std::string buffer = "ab";
    size_t consumed = 0;
    EXPECT_FALSE(TryDecodeFrame(buffer, consumed).has_value());
}

TEST(FrameUnitTest, TryDecodeReturnsNulloptWhenPayloadIncomplete) {
    std::string full_frame = EncodeFrame("hello world");
    std::string partial = full_frame.substr(0, full_frame.size() - 3);

    size_t consumed = 0;
    EXPECT_FALSE(TryDecodeFrame(partial, consumed).has_value());
}

TEST(FrameUnitTest, TryDecodeExtractsExactlyOneCompleteFrame) {
    std::string buffer = EncodeFrame("payload-one") + EncodeFrame("payload-two");

    size_t consumed = 0;
    auto first = TryDecodeFrame(buffer, consumed);
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(*first, "payload-one");
    buffer.erase(0, consumed);

    auto second = TryDecodeFrame(buffer, consumed);
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(*second, "payload-two");
    buffer.erase(0, consumed);

    EXPECT_TRUE(buffer.empty());
}

TEST(FrameUnitTest, RoundTripsEmptyPayload) {
    std::string buffer = EncodeFrame("");
    size_t consumed = 0;
    auto payload = TryDecodeFrame(buffer, consumed);
    ASSERT_TRUE(payload.has_value());
    EXPECT_EQ(*payload, "");
    EXPECT_EQ(consumed, buffer.size());
}

TEST(MessageWireFormatUnitTest, RoundTripsMessageWithFieldsIntact) {
    auto message = PubsubMessage{"message-id", "family-id", "the-payload"};

    auto decoded = DecodeMessage(EncodeMessage(message));

    EXPECT_EQ(decoded, message);
}

TEST(MessageWireFormatUnitTest, RoundTripsMessageWithEmptyFields) {
    auto message = PubsubMessage{"", "", ""};

    auto decoded = DecodeMessage(EncodeMessage(message));

    EXPECT_EQ(decoded, message);
}

TEST(MessageWireFormatUnitTest, ThrowsOnTrailingGarbageAfterMessage) {
    auto message = PubsubMessage{"id", "family", "payload"};
    auto encoded = EncodeMessage(message);
    encoded += "trailing-garbage";

    EXPECT_THROW(DecodeMessage(encoded), MalformedFrameException);
}

TEST(MessageWireFormatUnitTest, ThrowsOnTruncatedMessage) {
    auto message = PubsubMessage{"id", "family", "payload"};
    auto encoded = EncodeMessage(message);
    encoded.resize(encoded.size() / 2);

    EXPECT_THROW(DecodeMessage(encoded), MalformedFrameException);
}

TEST(SubscriptionRequestWireFormatUnitTest, RoundTripsRequest) {
    auto request = SubscriptionRequest{"subscriber-1", "family-1"};

    auto decoded = DecodeSubscriptionRequest(EncodeSubscriptionRequest(request));

    EXPECT_EQ(decoded, request);
}

TEST(SubscriptionRequestWireFormatUnitTest, ThrowsOnTrailingGarbage) {
    auto request = SubscriptionRequest{"subscriber-1", "family-1"};
    auto encoded = EncodeSubscriptionRequest(request);
    encoded += "x";

    EXPECT_THROW(DecodeSubscriptionRequest(encoded), MalformedFrameException);
}

TEST(SubscriptionRequestWireFormatUnitTest, ThrowsOnTruncatedRequest) {
    auto request = SubscriptionRequest{"subscriber-1", "family-1"};
    auto encoded = EncodeSubscriptionRequest(request);
    encoded.resize(2);

    EXPECT_THROW(DecodeSubscriptionRequest(encoded), MalformedFrameException);
}

} // namespace
