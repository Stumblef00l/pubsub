#ifndef _SUBSCRIBER_FAMILY_MANAGER_HPP_
#define _SUBSCRIBER_FAMILY_MANAGER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "subscriber_family.hpp"

namespace pubsub {
    class ISubscriberFamilyManager {
        public:
            typedef std::unique_ptr<ISubscriberFamily> SubscriberFamilyManagerPtr;

            virtual void createSubscriberFamily(std::unique_ptr<ISubscriberFamily> subscriberFamily) = 0;
            virtual void deleteSubscriberFamily(const ISubscriberFamily::SubscriberFamilyID& familyID) = 0;
            virtual ISubscriberFamily* getSubscriberFamily(const ISubscriberFamily::SubscriberFamilyID& familyID) const = 0;

        protected:
            std::vector<SubscriberFamilyManagerPtr> subscriberFamilyList_;
            ISubscriberFamilyManager();
    };

    class BasicSubscriberFamilyManager: public ISubscriberFamilyManager {
        public:
            BasicSubscriberFamilyManager();

            void createSubscriberFamily(std::unique_ptr<ISubscriberFamily> subscriberFamily) override;
            void deleteSubscriberFamily(const ISubscriberFamily::SubscriberFamilyID& familyID) override;
            ISubscriberFamily* getSubscriberFamily(const ISubscriberFamily::SubscriberFamilyID& familyID) const override;
    };


    // Thrown when trying to create a SubscriberFamily that already exists
    class SubscriberFamilyAlreadyExistsException: public std::exception {
        static constexpr std::string_view ERROR_MESSAGE = "Create failed as SubscriberFamily with provided ID already exists";
        
        public:
            // Displays the error message
            inline const char*
            what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
                return ERROR_MESSAGE.data();
            };
    };

    // Thrown when a requested SubscriberFamily does not exist
    class SubscriberFamilyNotFoundException: public std::exception {
        static constexpr std::string_view ERROR_MESSAGE = "SubscriberFamily not found registered to manager";
        
        public:
            // Displays the error message
            inline const char*
            what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
                return ERROR_MESSAGE.data();
            };
    };
}

#endif