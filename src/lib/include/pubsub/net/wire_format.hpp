#ifndef _PUBSUB_NET_WIRE_FORMAT_HPP_
#define _PUBSUB_NET_WIRE_FORMAT_HPP_

#include <exception>
#include <optional>
#include <string>
#include <string_view>

#include "pubsub/structs.hpp"

namespace pubsub {
namespace net {
namespace wire_format {

// Appends a 4-byte (network byte order) length prefix followed by value's
// bytes to out.
void
AppendLengthPrefixed(
    std::string& out,
    std::string_view value);

// Reads a length-prefixed string starting at offset within buffer, advancing
// offset past the bytes consumed. Throws MalformedFrameException if buffer
// does not contain a complete, well-formed length-prefixed value at offset.
std::string
ReadLengthPrefixed(
    const std::string& buffer,
    size_t& offset);

// Wraps an opaque payload with a 4-byte length prefix, suitable for framing
// arbitrary payloads over a byte stream (e.g. a TCP connection).
std::string
EncodeFrame(
    std::string_view payload);

// Attempts to extract one complete frame from the front of buffer. Returns
// std::nullopt (leaving buffer untouched) if buffer does not yet contain a
// complete frame. On success, consumed_bytes is set to the number of bytes
// (including the length prefix) that the caller should erase from the front
// of buffer.
std::optional<std::string>
TryDecodeFrame(
    const std::string& buffer,
    size_t& consumed_bytes);

// Serializes/deserializes a PubsubMessage to/from a frame payload.
std::string
EncodeMessage(
    const PubsubMessage& message);

PubsubMessage
DecodeMessage(
    const std::string& frame_payload);

// Sent once by a NetworkSubscriberClient immediately after connecting, to
// tell the NetworkPublisherService which subscriber id/family it represents.
struct SubscriptionRequest {
    PubsubSubscriberId subscriber_id;
    PubsubSubscriberFamilyId family_id;

    bool operator== (
        const SubscriptionRequest& other) const {
        return (subscriber_id == other.subscriber_id)
            && (family_id == other.family_id);
    }
};

std::string
EncodeSubscriptionRequest(
    const SubscriptionRequest& request);

SubscriptionRequest
DecodeSubscriptionRequest(
    const std::string& frame_payload);

// Thrown when decoding encounters truncated, oversized, or otherwise
// malformed input.
class MalformedFrameException: public std::exception {
    public:
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };

    private:
        static constexpr std::string_view errorMessage = "Malformed or truncated wire frame";
};

} // namespace wire_format
} // namespace net
} // namespace pubsub

#endif
