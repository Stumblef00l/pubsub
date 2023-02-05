#ifndef _STRUCTS_
#define _STRUCTS_

#include <string>

typedef std::string PubsubMessageId;
typedef std::string PubsubSubscriberFamilyId;
typedef std::string PubsubPayload;

struct PubsubMessage {
    PubsubMessageId id;
    PubsubSubscriberFamilyId familyID;
    PubsubPayload payload;

    PubsubMessage(PubsubMessage&& message)
    : id(std::move(message.id)),
      familyID(std::move(message.familyID)),
      payload(std::move(message.payload)) {}
    
    PubsubMessage(const PubsubMessage& message)
    : id(message.id),
      familyID(message.familyID),
      payload(message.payload) {}

    PubsubMessage& operator= (PubsubMessage&& message) {
        id = std::move(message.id);
        familyID = std::move(message.familyID);
        payload = std::move(message.payload);
        return *this;
    }
    
    PubsubMessage& operator= (const PubsubMessage& message) {
        id = message.id;
        familyID = message.familyID;
        payload = message.payload;
        return *this;
    }
};

#endif