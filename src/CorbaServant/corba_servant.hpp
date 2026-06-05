#pragma once

#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>
#include "MMCommonC.h"
#include "monitor_consumer.hpp"
#include "../SubscriptionRegistry/subscription_registry.hpp"
#include "stream.pb.h"


class CorbaServant {
    public:
        using MonitorRegistry = SubscriptionRegistry<gateway::MonitorEvent>;

        CorbaServant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, MonitorRegistry& registry);
        ~CorbaServant();

        MM::Consumer_ifce_ptr getMonitorConsumerObject() const;
    private:
        void activateMonitorConsumer();

        CORBA::ORB_var orb_;
        PortableServer::POA_var poa_;

        MonitorConsumer* monitor_consumer_impl_;
        MM::Consumer_ifce_var monitor_consumer_ref_;
        MonitorRegistry& monitor_registry_;
};