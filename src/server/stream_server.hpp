#pragma once
 
#include <grpcpp/grpcpp.h>
#include "server.grpc.pb.h"
#include "stream.pb.h"
 
#include "corba_runtime.hpp"
#include "../CorbaServant/corba_servant.hpp"
#include "../SubscriptionPropagator/monitor_propagator.hpp"
#include "../SubscriptionPropagator/alarm_propagator.hpp"
#include "../SubscriptionPropagator/log_propagator.hpp"


class Stream {
    public:
        explicit Stream(CorbaRuntime& corba): 
            corba_(corba),
            corba_servant_(corba.orb(), corba.rootPoa(), 
                monitor_registry_, alarm_registry_, log_registry_),
            
            monitor_propagator_(corba.ns(), monitor_registry_),
            alarm_propagator_(corba.ns(), alarm_registry_),
            log_propagator_(corba.ns(), log_registry_)
        {}
        
        grpc::Status SubscribeMonitor(
            grpc::ServerContext* context,
            const gateway::MonitorReq* request,
            grpc::ServerWriter<gateway::MonitorEvent>* writer
        );

        grpc::Status SubscribeAlarms(
            grpc::ServerContext* context,
            const gateway::AlarmReq* request,
            grpc::ServerWriter<gateway::AlarmEvent>* writer
        );

        grpc::Status SubscribeLogs(
            grpc::ServerContext* context,
            const gateway::LogReq* request,
            grpc::ServerWriter<gateway::LogEvent>* writer
        );
    
    private:
    CorbaRuntime& corba_;
    CorbaServant corba_servant_;

    CorbaServant::MonitorRegistry monitor_registry_;
    CorbaServant::AlarmRegistry alarm_registry_;
    CorbaServant::LogRegistry log_registry_;
    
    MonitorPropagator monitor_propagator_;
    AlarmPropagator alarm_propagator_;
    LogPropagator log_propagator_;
};