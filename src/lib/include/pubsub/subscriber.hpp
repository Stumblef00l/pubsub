#ifndef _SUBSCRIBER_HPP_
#define _SUBSCRIBER_HPP_

#include "structs.hpp"

namespace pubsub {

class ISubscriber {
    public:
        typedef std::string SubscriberID;

        virtual SubscriberID GetID() const = 0; 
        virtual void Update(PubsubMessage message) = 0;

    protected:
        ISubscriber(const SubscriberID id);

        const SubscriberID id_;
};

} // namespace pubsub

#endif