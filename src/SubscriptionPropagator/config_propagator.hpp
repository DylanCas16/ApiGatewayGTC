#pragma once

#include <tao/corba.h>
#include "subscription_propagator.hpp"
#include "CONFIGC.h"
#include "CONFIGManagerC.h"
#include "stream.pb.h"


class ConfigPropagator : public SubscriptionPropagator<gateway::ConfigEvent> {
    public:
        using Base = SubscriptionPropagator<gateway::ConfigEvent>;

        ConfigPropagator(NsResolver& ns, Base::Registry& registry);

        uint64_t subscribe(const std::string& property,
                            CONFIG::Consumer_ifce_ptr consumer,
                            grpc::ServerWriter<gateway::ConfigEvent>* writer
        );

        void unsubscribe(uint64_t id, const std::string& property,
                            CONFIG::Consumer_ifce_ptr consumer
        );

    protected:
        CORBA::Object_ptr propagatorObject() const override { return propagator_.in(); }
        void narrowCorbaObject(CORBA::Object_ptr obj) override;

    private:
        void corbaSubscribe(CONFIG::Consumer_ifce_ptr consumer);
        void corbaUnsubscribe(CONFIG::Consumer_ifce_ptr consumer);

        CONFIG::Manager_ifce_var propagator_;
};