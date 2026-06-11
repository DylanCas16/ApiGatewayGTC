#pragma once

#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>
#include "../SubscriptionRegistry/subscription_registry.hpp"
#include "stream.pb.h"

#include "MMCommonC.h"
#include "ALARMC.h"
#include "LOGC.h"
#include "monitor_consumer.hpp"
#include "alarm_consumer.hpp"
#include "log_consumer.hpp"



class CorbaServant {
    public:
        using MonitorRegistry = SubscriptionRegistry<gateway::MonitorEvent>;
        using AlarmRegistry = SubscriptionRegistry<gateway::AlarmEvent>;
        using LogRegistry = SubscriptionRegistry<gateway::LogEvent>;

        CorbaServant(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa,
                        MonitorRegistry& monitor_registry,
                        AlarmRegistry& alarm_registry,
                        LogRegistry& log_registry);
        ~CorbaServant();

        MM::Consumer_ifce_ptr getMonitorConsumerObject() const;
        ALARM::Consumer_ifce_ptr getAlarmConsumerObject() const;
        LOG::Consumer_ifce_ptr getLogConsumerObject() const;

    private:
        CORBA::Object_var activateServantObject(PortableServer::ServantBase* impl);
        void deactivateServant(PortableServer::ServantBase* impl, const char* name);
        
        void activateMonitorConsumer();
        void activateAlarmConsumer();
        void activateLogConsumer();

        CORBA::ORB_var orb_;
        PortableServer::POA_var poa_;

        MonitorConsumer* monitor_consumer_impl_;
        AlarmConsumer* alarm_consumer_impl_;
        LogConsumer* log_consumer_impl_;

        MM::Consumer_ifce_var monitor_consumer_ref_;
        ALARM::Consumer_ifce_var alarm_consumer_ref_;
        LOG::Consumer_ifce_var log_consumer_ref_;

        MonitorRegistry& monitor_registry_;
        AlarmRegistry& alarm_registry_;
        LogRegistry& log_registry_;
};