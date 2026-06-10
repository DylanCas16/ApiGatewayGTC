#pragma once

#include "subscription_propagator.hpp"
#include "MMCommonC.h"
#include "stream.pb.h"
#include <tao/corba.h>


class MonitorPropagator : public SubscriptionPropagator<gateway::MonitorEvent> {
    public:
        using Base = SubscriptionPropagator<gateway::MonitorEvent>;

        MonitorPropagator(NsResolver& ns, Base::Registry& registry);
        
        uint64_t subscribe(const std::string& component, const std::string& magnitude,
                            gateway::MonitorType type, MM::Consumer_ifce_ptr consumer,
                            grpc::ServerWriter<gateway::MonitorEvent>* writer
        );

        void unsubscribe(uint64_t id, const std::string& component, const std::string& magnitude,
                            gateway::MonitorType type, MM::Consumer_ifce_ptr consumer
        );
    
    protected:
        CORBA::Object_ptr propagatorObject() const override { return propagator_.in(); }
        void narrowCorbaObject(CORBA::Object_ptr obj) override;

    private:
        void corbaSubscribe(const std::string& component, const std::string& magnitude, 
                            gateway::MonitorType type, MM::Consumer_ifce_ptr consumer
        );
        
        void corbaUnsubscribe(const std::string& component, const std::string& magnitude, 
                            gateway::MonitorType type, MM::Consumer_ifce_ptr consumer
        );

        MM::MonitorPropagator_ifce_var propagator_;
};
