#ifndef _SUBSCRIBER_HPP_
#define _SUBSCRIBER_HPP_

#include "structs.hpp"

class ISubscriber {
    public:
        virtual void update(PubsubMessage message) = 0;
};

#endif