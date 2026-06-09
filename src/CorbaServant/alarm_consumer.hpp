#pragma once

#include <tao/corba.h>
#include "ALARMS.h"
#include "ALARMC.h"
#include "../adapter/alarm_adapter.hpp"
#include "../SubscriptionRegistry/subscription_registry.hpp"
#include "stream.pb.h"


class AlarmConsumer : public POA_ALARM::Consumer_ifce {
    public:
        using Registry = SubscriptionRegistry<gateway::AlarmEvent>;

        explicit AlarmConsumer(Registry& registry);
        virtual ~AlarmConsumer() = default;

        virtual void receiveAlarms(const ALARM::EventList& event_list);
    private:
        Registry& registry_;
};