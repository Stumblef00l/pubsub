#ifndef _SUBSCRIBER_FAMILY_MANAGER_HPP_
#define _SUBSCRIBER_FAMILY_MANAGER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "structs.hpp"
#include "subscriber_family.hpp"

namespace pubsub {

class ISubscriberFamilyManager {
    public:
        typedef std::unique_ptr<ISubscriberFamily> ISubscriberFamilyUniquePtr;

        virtual void CreateFamily(ISubscriberFamilyUniquePtr subscriber_family);
        virtual void DeleteFamily(const PubsubSubscriberFamilyId& family_id);
        virtual ISubscriberFamily* GetFamily(const PubsubSubscriberFamilyId& family_id) const;

        virtual ~ISubscriberFamilyManager() {}

    protected:
        ISubscriberFamilyManager();

        std::vector<ISubscriberFamilyUniquePtr> subscriber_family_list_;
};

class BasicSubscriberFamilyManager: public ISubscriberFamilyManager {
    public:
        BasicSubscriberFamilyManager();
};


// Thrown when trying to create a SubscriberFamily that already exists
class SubscriberFamilyAlreadyExistsException: public std::exception {
    static constexpr std::string_view errorMessage = "Create failed as SubscriberFamily with provided ID already exists";
    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
};

// Thrown when a requested SubscriberFamily does not exist
class SubscriberFamilyNotFoundException: public std::exception {
    static constexpr std::string_view errorMessage = "SubscriberFamily not found registered to manager";
    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
};

} // namespace pubsub

#endif