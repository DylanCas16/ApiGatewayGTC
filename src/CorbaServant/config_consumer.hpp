#pragma once

#include <tao/corba.h>
#include "CONFIGS.h"
#include "CONFIGC.h"
#include "../adapter/config_adapter.hpp"
#include "../SubscriptionRegistry/subscription_registry.hpp"
#include "stream.pb.h"


class ConfigConsumer : public POA_CONFIG::Consumer_ifce {
    public:
        using Registry = SubscriptionRegistry<gateway::ConfigEvent>;

        explicit ConfigConsumer(Registry& registry);
        virtual ~ConfigConsumer() = default;

        virtual void receivePropertyChanges(const CONFIG::PropertyChangeList& property_list);
    private:
        Registry& registry_;
};