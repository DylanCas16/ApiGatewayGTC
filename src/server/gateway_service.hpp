#pragma once
 
#include <grpcpp/grpcpp.h>
#include "server.grpc.pb.h"
#include "corba_runtime.hpp"
#include "unary_server.hpp"
#include "stream_server.hpp"
// #include "config_server.hpp"


class GatewayService final : public gateway::GatewayServer::Service {
public:
    explicit GatewayService(CorbaRuntime& corba)
        : unary_(corba), stream_(corba)
        //, config_(corba)
    {}
 
    grpc::Status Invoke(
        grpc::ServerContext* context,
        const gateway::InvokeRequest* request,
        gateway::InvokeResponse* response
    ) override;
 
    grpc::Status Resolve(
        grpc::ServerContext* context,
        const gateway::ResolveRequest* request,
        gateway::ComponentInfo* response
    ) override;
 
    grpc::Status ListNaming(
        grpc::ServerContext* context,
        const gateway::NamingRequest* request,
        gateway::NamingResponse* response
    ) override;
/*
    grpc::Status GetProperty(
        grpc::ServerContext* context,
        const gateway::GetPropRequest* request,
        gateway::PropertyValue* response
    ) override;

    grpc::Status SetProperty(
        grpc::ServerContext* context,
        const gateway::SetPropRequest* request,
        grpc::SetPropResponse* response
    ) override;

    grpc::Status SubscribeConfig(
        grpc::ServerContext* context,
        const gateway::ConfigReq* request,
        grpc::ServerWriter<gateway::ConfigEvent>* writer
    ) override;
*/
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
    Unary  unary_;
    Stream stream_;
    // Config config_;
};