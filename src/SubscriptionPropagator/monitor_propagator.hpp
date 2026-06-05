#pragma once

#include <tao/corba.h>
#include <string>
#include <stdint.h>
#include "MMCommonC.h"
#include "MonitorManagerC.h"
#include "../SubscriptionRegistry/subscription_registry.hpp"
#include "../corba/corba_runtime.hpp"
#include "stream.pb.h"


class MonitorPropagator {
    public:
        using Registry = SubscriptionRegistry<gateway::MonitorEvent>;

        MonitorPropagator(NsResolver& ns, Registry& registry);
        
        uint64_t subscribe(const std::string& component, const std::string& magnitude,
                            gateway::MonitorType type, MM::Consumer_ifce_ptr consumer,
                            grpc::ServerWriter<gateway::MonitorEvent>* writer
        );

        void unsubscribe(uint64_t id, const std::string& component, const std::string& magnitude,
                            gateway::MonitorType type, MM::Consumer_ifce_ptr consumer
        );

        bool isConnected() const { return !CORBA::is_nil(propagator_.in()); }
    
    private:
        void ensureConnected();

        void corbaSubscribe(const std::string& component, const std::string& magnitude, 
                            gateway::MonitorType type, MM::Consumer_ifce_ptr consumer
        );
        
        void corbaUnsubscribe(const std::string& component, const std::string& magnitude, 
                            gateway::MonitorType type, MM::Consumer_ifce_ptr consumer
        );

        NsResolver& ns_;
        Registry& registry_;
        MM::MonitorPropagator_ifce_var propagator_;
};
