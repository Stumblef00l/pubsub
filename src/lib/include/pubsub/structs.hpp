#ifndef _STRUCTS_
#define _STRUCTS_

#include <string>

namespace pubsub {

typedef std::string PubsubMessageId;
typedef std::string PubsubSubscriberFamilyId;
typedef std::string PubsubPayload;

struct PubsubMessage {
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
    PubsubPayload payload;
};

} // namespace pubsub

#endif