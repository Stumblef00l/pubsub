#ifndef _SUBSCRIBER_FAMILY_MANAGER_HPP_
#define _SUBSCRIBER_FAMILY_MANAGER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "subscriber_family.hpp"

namespace pubsub {

class ISubscriberFamilyManager {
    public:
        typedef std::unique_ptr<ISubscriberFamily> ISubscriberFamilyUniquePtr;

        virtual void CreateSubscriberFamily(ISubscriberFamilyUniquePtr subscriber_family) = 0;
        virtual void DeleteSubscriberFamily(const ISubscriberFamily::SubscriberFamilyID& family_id) = 0;
        virtual ISubscriberFamily* GetSubscriberFamily(const ISubscriberFamily::SubscriberFamilyID& family_id) const = 0;

    protected:
        ISubscriberFamilyManager();

        std::vector<ISubscriberFamilyUniquePtr> subscriber_family_list_;
};

class BasicSubscriberFamilyManager: public ISubscriberFamilyManager {
    public:
        BasicSubscriberFamilyManager();

        void CreateSubscriberFamily(ISubscriberFamilyUniquePtr subscriber_family) override;
        void DeleteSubscriberFamily(const ISubscriberFamily::SubscriberFamilyID& family_id) override;
        ISubscriberFamily* GetSubscriberFamily(const ISubscriberFamily::SubscriberFamilyID& family_id) const override;
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