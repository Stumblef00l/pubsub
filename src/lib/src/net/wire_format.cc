#include "pubsub/net/wire_format.hpp"

#include <arpa/inet.h>

#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "pubsub/structs.hpp"

namespace pubsub {
namespace net {
namespace wire_format {

namespace {

constexpr size_t kLengthPrefixSize = sizeof(uint32_t);

} // namespace

void
AppendLengthPrefixed(
    std::string& out,
    std::string_view value) {
    uint32_t length = htonl(static_cast<uint32_t>(value.size()));
    out.append(reinterpret_cast<const char*>(&length), kLengthPrefixSize);
    out.append(value.data(), value.size());
}

std::string
ReadLengthPrefixed(
    const std::string& buffer,
    size_t& offset) {
    if (buffer.size() < offset + kLengthPrefixSize)
        throw MalformedFrameException();

    uint32_t network_length;
    std::memcpy(&network_length, buffer.data() + offset, kLengthPrefixSize);
    uint32_t length = ntohl(network_length);
    offset += kLengthPrefixSize;

    if (buffer.size() < offset + length)
        throw MalformedFrameException();

    std::string value = buffer.substr(offset, length);
    offset += length;
    return value;
}

std::string
EncodeFrame(
    std::string_view payload) {
    std::string out;
    out.reserve(kLengthPrefixSize + payload.size());
    AppendLengthPrefixed(out, payload);
    return out;
}

std::optional<std::string>
TryDecodeFrame(
    const std::string& buffer,
    size_t& consumed_bytes) {
    if (buffer.size() < kLengthPrefixSize)
        return std::nullopt;

    uint32_t network_length;
    std::memcpy(&network_length, buffer.data(), kLengthPrefixSize);
    uint32_t length = ntohl(network_length);

    if (buffer.size() < kLengthPrefixSize + length)
        return std::nullopt;

    consumed_bytes = kLengthPrefixSize + length;
    return buffer.substr(kLengthPrefixSize, length);
}

std::string
EncodeMessage(
    const PubsubMessage& message) {
    std::string out;
    AppendLengthPrefixed(out, message.id);
    AppendLengthPrefixed(out, message.family_id);
    AppendLengthPrefixed(out, message.payload);
    return out;
}

PubsubMessage
DecodeMessage(
    const std::string& frame_payload) {
    size_t offset = 0;
    auto id = ReadLengthPrefixed(frame_payload, offset);
    auto family_id = ReadLengthPrefixed(frame_payload, offset);
    auto payload = ReadLengthPrefixed(frame_payload, offset);

    if (offset != frame_payload.size())
        throw MalformedFrameException();

    return PubsubMessage(std::move(id), std::move(family_id), std::move(payload));
}

std::string
EncodeSubscriptionRequest(
    const SubscriptionRequest& request) {
    std::string out;
    AppendLengthPrefixed(out, request.subscriber_id);
    AppendLengthPrefixed(out, request.family_id);
    return out;
}

SubscriptionRequest
DecodeSubscriptionRequest(
    const std::string& frame_payload) {
    size_t offset = 0;
    auto subscriber_id = ReadLengthPrefixed(frame_payload, offset);
    auto family_id = ReadLengthPrefixed(frame_payload, offset);

    if (offset != frame_payload.size())
        throw MalformedFrameException();

    return SubscriptionRequest{std::move(subscriber_id), std::move(family_id)};
}

} // namespace wire_format
} // namespace net
} // namespace pubsub
