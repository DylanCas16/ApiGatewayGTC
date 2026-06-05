#pragma once
 
#include <grpcpp/grpcpp.h>
#include <thread>
#include <chrono>
 
#include "server.grpc.pb.h"
#include "stream.pb.h"
 
#include "corba_runtime.hpp"
#include "../CorbaServant/corba_servant.hpp"
#include "../SubscriptionPropagator/monitor_propagator.hpp"


class Stream final : public gateway::GatewayServer::Service {
    public:
        explicit Stream(CorbaRuntime& corba): 
            corba_(corba),
            corba_servant_(corba.orb(), corba.rootPoa(), monitor_registry_),
            monitor_propagator_(corba.ns(), monitor_registry_) 
        {}
        
        grpc::Status SubscribeMonitor(
            grpc::ServerContext* context,
            const gateway::MonitorReq* request,
            grpc::ServerWriter<gateway::MonitorEvent>* writer
        ) override;

        grpc::Status SubscribeAlarms(
            grpc::ServerContext* context,
            const gateway::AlarmReq* request,
            grpc::ServerWriter<gateway::AlarmEvent>* writer
        ) override;

        grpc::Status SubscribeLogs(
            grpc::ServerContext* context,
            const gateway::LogReq* request,
            grpc::ServerWriter<gateway::LogEvent>* writer
        ) override;
    
    private:
        CorbaServant::MonitorRegistry monitor_registry_;
        CorbaRuntime& corba_;
        CorbaServant corba_servant_;
        MonitorPropagator monitor_propagator_;
};