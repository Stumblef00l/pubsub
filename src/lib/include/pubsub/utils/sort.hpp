#ifndef _PUBSUB_UTILS_SORT_HPP_
#define _PUBSUB_UTILS_SORT_HPP_

#include <utility>
#include <string>

#include "pubsub/subscriber.hpp"

namespace pubsub {
namespace utils {

bool CompareSubscribers(const ISubscriber* a, const ISubscriber* b) {
    return ((a->GetID()).compare(b->GetID()) < 0);
}

} // namespace utils
} // namespace pubsub

#endif