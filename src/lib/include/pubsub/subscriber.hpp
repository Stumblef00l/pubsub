#ifndef _SUBSCRIBER_HPP_
#define _SUBSCRIBER_HPP_

#include "structs.hpp"

namespace pubsub {

class ISubscriber {
    public:
        typedef std::string SubscriberID;

        virtual SubscriberID getID() const = 0; 
        virtual void update(PubsubMessage message) = 0;
    protected:
        ISubscriber(const SubscriberID& id);

        const SubscriberID id;
};

} // namespace pubsub

#endif