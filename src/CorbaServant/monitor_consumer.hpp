#pragma once

#include <tao/corba.h>
#include "MMCommonS.h"
#include "MMCommonC.h"
#include "../adapter/monitor_adapter.hpp"
#include "../SubscriptionRegistry/subscription_registry.hpp"
#include "stream.pb.h"


class MonitorConsumer : public POA_MM::Consumer_ifce {
    public:
        using Registry = SubscriptionRegistry<gateway::MonitorEvent>;

        explicit MonitorConsumer(Registry& registry);
        virtual ~MonitorConsumer() = default;

        virtual void receiveDataBlocks(const MM::DataBlockList& data_list);
        virtual void receiveStateChanges(const MM::StateChangeList& state_list);
        virtual void receiveMagnitudeChanges(const MM::MagnitudeChangeList& magnitude_list);
    private:
        Registry& registry_;
};