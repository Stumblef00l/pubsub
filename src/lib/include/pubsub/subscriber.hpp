#ifndef _SUBSCRIBER_HPP_
#define _SUBSCRIBER_HPP_

#include "structs.hpp"

namespace pubsub {

class ISubscriber {
    public:
        virtual PubsubSubscriberId GetID() const; 
        virtual void Update(PubsubMessage message) = 0;

        virtual ~ISubscriber() {}

    protected:
        ISubscriber(PubsubSubscriberId id);

        const PubsubSubscriberId id_;
};

} // namespace pubsub

#endif