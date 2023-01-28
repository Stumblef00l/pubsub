#ifndef _STRUCTS_
#define _STRUCTS_

#include <string>

typedef std::string PubsubMessageId;
typedef std::string PubsubSubscriberFamilyId;

struct PubsubPayload {
    std::string data;
};

struct PubsubMessage {
    PubsubMessageId id;
    PubsubSubscriberFamilyId familyID;
    PubsubPayload payload;
};

#endif