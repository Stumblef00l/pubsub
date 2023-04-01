#ifndef _PUBSUB_SUBSCRIBER_FAMILY_MANAGER_HPP_
#define _PUBSUB_SUBSCRIBER_FAMILY_MANAGER_HPP_

#include <exception>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

#include "structs.hpp"
#include "subscriber_family.hpp"

namespace pubsub {

class ISubscriberFamilyManager {
    public:
        typedef std::unique_ptr<ISubscriberFamily> ISubscriberFamilyUniquePtr;

        virtual void
        CreateFamily(
            ISubscriberFamilyUniquePtr subscriber_family);

        virtual void
        DeleteFamily(
            const PubsubSubscriberFamilyId& family_id);

        virtual ISubscriberFamily*
        GetFamily(
            const PubsubSubscriberFamilyId& family_id) const;

        virtual ~ISubscriberFamilyManager() {}

    protected:
        ISubscriberFamilyManager();

        std::vector<ISubscriberFamilyUniquePtr> subscriber_family_list_;
};

class IThreadSafeSubscriberFamilyManager: public ISubscriberFamilyManager {
    public:
        virtual
        ~IThreadSafeSubscriberFamilyManager() noexcept {}
    
    protected:
        IThreadSafeSubscriberFamilyManager();
};

class BasicSubscriberFamilyManager: public ISubscriberFamilyManager {
    public:
        BasicSubscriberFamilyManager();
};

class BasicThreadSafeSubscriberFamilyManager: public IThreadSafeSubscriberFamilyManager {
    public:
        BasicThreadSafeSubscriberFamilyManager();

        void
        CreateFamily(
            ISubscriberFamilyUniquePtr subscriber_family) override;
        
        void
        DeleteFamily(
            const PubsubSubscriberFamilyId& family_id) override;
        
        ISubscriberFamily*
        GetFamily(
            const PubsubSubscriberFamilyId& family_id) const override;
    
    private:
        mutable std::shared_mutex subscriber_family_list_mtx_;
};


// Thrown when trying to create a SubscriberFamily that already exists
class SubscriberFamilyAlreadyExistsException: public std::exception {
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
    
    private:
        static constexpr std::string_view errorMessage = "Create failed as SubscriberFamily with provided ID already exists";
};

// Thrown when a requested SubscriberFamily does not exist
class SubscriberFamilyNotFoundException: public std::exception {
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
    
    private:
        static constexpr std::string_view errorMessage = "SubscriberFamily not found registered to manager";
};

} // namespace pubsub

#endif