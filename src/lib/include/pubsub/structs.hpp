#ifndef _PUBSUB_STRUCTS_
#define _PUBSUB_STRUCTS_

#include <string>

namespace pubsub {

// Unique key of a pubsub message
typedef std::string PubsubMessageId;

// Unique ID to identify a subscriber family
typedef std::string PubsubSubscriberFamilyId;

// Pubsub payload.
typedef std::string PubsubMessagePayload;

// Unique ID to identify a subscriber
typedef std::string PubsubSubscriberId;

struct PubsubMessage {
    PubsubMessage(
      PubsubMessageId id,
      PubsubSubscriberFamilyId family_id,
      PubsubMessagePayload payload)
      : id(std::move(id)),
        family_id(std::move(family_id)),
        payload(std::move(payload)) {}

    PubsubMessage(PubsubMessage&& message)
    : id(std::move(message.id)),
      family_id(std::move(message.family_id)),
      payload(std::move(message.payload)) {}
    
    PubsubMessage(const PubsubMessage& message)
    : id(message.id),
      family_id(message.family_id),
      payload(message.payload) {}

    PubsubMessage& operator= (PubsubMessage&& message) {
        id = std::move(message.id);
        family_id = std::move(message.family_id);
        payload = std::move(message.payload);
        return *this;
    }
    
    PubsubMessage& operator= (const PubsubMessage& message) {
        id = message.id;
        family_id = message.family_id;
        payload = message.payload;
        return *this;
    }

    PubsubMessageId id;
    PubsubSubscriberFamilyId family_id;
    PubsubMessagePayload payload;
};

} // namespace pubsub

#endif