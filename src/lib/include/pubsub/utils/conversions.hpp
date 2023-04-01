#ifndef _PUBSUB_UTILS_CONVERSIONS_HPP_
#define _PUBSUB_UTILS_CONVERSIONS_HPP_

#include <memory>
#include <vector>

namespace pubsub {
namespace utils {

template<class T>
std::vector<T*>
ConvertUniquePtrListToRawPointerList(
    const std::vector<std::unique_ptr<T>>& uptr_list) {
    auto raw_pointer_list = std::vector<T*>{};
    for(auto& uptr: uptr_list)
        raw_pointer_list.push_back(uptr.get());
    return raw_pointer_list;
}

} // namespace utils
} // namespace pubsub

#endif
