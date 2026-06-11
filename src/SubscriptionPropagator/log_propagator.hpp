#pragma once

#include <tao/corba.h>
#include "subscription_propagator.hpp"
#include "LOGC.h"
#include "LOGManagerC.h"
#include "stream.pb.h"


class LogPropagator : public SubscriptionPropagator<gateway::LogEvent> {
    public:
        using Base = SubscriptionPropagator<gateway::LogEvent>;

        LogPropagator(NsResolver& ns, Base::Registry& registry);

        uint64_t subscribe(const std::string& component,
                            LOG::Consumer_ifce_ptr consumer,
                            grpc::ServerWriter<gateway::AlarmEvent>* writer
        );

        void unsubscribe(uint64_t id, const std::string& component,
                            LOG::Consumer_ifce_ptr consumer
        );

    protected:
        CORBA::Object_ptr propagatorObject() const override { return propagator_.in(); }
        void narrowCorbaObject(CORBA::Object_ptr obj) override;

    private:
        void corbaSubscribe(LOG::Consumer_ifce_ptr consumer);
        void corbaUnsubscribe(LOG::Consumer_ifce_ptr consumer);

        LOG::Manager_ifce_var propagator_;
};