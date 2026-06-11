#pragma once

#include <tao/corba.h>
#include "LOGS.h"
#include "LOGC.h"
#include "../adapter/log_adapter.hpp"
#include "../SubscriptionRegistry/subscription_registry.hpp"
#include "stream.pb.h"


class LogConsumer : public POA_LOG::Consumer_ifce {
    public:
        using Registry = SubscriptionRegistry<gateway::LogEvent>;

        explicit LogConsumer(Registry& registry);
        virtual ~LogConsumer() = default;

        virtual void receiveLogMessages(const LOG::RecordList& record_list) override;
    private:
        Registry& registry_;
};