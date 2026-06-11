#pragma once

#include <string>
#include <tao/corba.h>
#include "../SubscriptionRegistry/subscription_registry.hpp"
#include "../corba/NamingService/ns_discover.hpp"

template<typename EventT>
class SubscriptionPropagator {
    public:
        using Registry = SubscriptionRegistry<EventT>;

        bool isConnected() const { return !CORBA::is_nil(propagatorObject()); }
    
    protected:
        SubscriptionPropagator(NsResolver& ns, Registry& registry, const std::string& ns_name) :
        ns_(ns), registry_(registry), ns_name_(ns_name) {}
        
        ~SubscriptionPropagator() = default;

        void ensureConnected() 
        {
            if (isConnected()) return;

            CORBA::Object_var obj = ns_.resolve(ns_name_);
            if (CORBA::is_nil(obj.in()))
                throw std::runtime_error("[SubscriptionPropagator] Not found in NS: " + ns_name_);

            narrowCorbaObject(obj.in());
        }

        virtual void narrowCorbaObject(CORBA::Object_ptr obj) = 0;
        virtual CORBA::Object_ptr propagatorObject() const = 0;

        static std::string makeTopic(const std::string& component,
                                  const std::string& magnitude)
        {
            return "T:" + component + ":" + magnitude;
        }

        NsResolver& ns_;
        Registry& registry_;
        std::string ns_name_;
};
