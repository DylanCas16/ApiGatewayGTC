#pragma once

#include <tao/corba.h>
#include "subscription_propagator.hpp"
#include "ALARMC.h"
#include "ALARMManagerC.h"
#include "stream.pb.h"


class AlarmPropagator : public SubscriptionPropagator<gateway::AlarmEvent> {
    public:
        using Base = SubscriptionPropagator<gateway::AlarmEvent>;

        AlarmPropagator(NsResolver& ns, Base::Registry& registry);

        uint64_t subscribe(const std::string& component,
                            ALARM::Consumer_ifce_ptr consumer,
                            grpc::ServerWriter<gateway::AlarmEvent>* writer
        );

        void unsubscribe(uint64_t id, const std::string& component,
                            ALARM::Consumer_ifce_ptr consumer
        );

    protected:
        CORBA::Object_ptr propagatorObject() const override { return propagator_.in(); }
        void narrowCorbaObject(CORBA::Object_ptr obj) override;

    private:
        void corbaSubscribe(ALARM::Consumer_ifce_ptr consumer);
        void corbaUnsubscribe(ALARM::Consumer_ifce_ptr consumer);

        ALARM::Manager_ifce_var propagator_;
};