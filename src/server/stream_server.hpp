#pragma once

#include <grpcpp/grpcpp.h>
#include "server.grpc.pb.h"
#include "corba_runtime.hpp"


class Stream final : public gateway::GatewayServer::Service {
    public:
        explicit Stream(CorbaRuntime& corba) : corba_(corba) {}
        
        grpc::Status MonitorManager(
            grpc::ServerContext* context,
            const grpc::MonitorReq* request,
            grpc::MonitorEvent* response
        ) override;

        grpc::Status AlarmManager(
            grpc::ServerContext* context,
            const grpc::AlarmReq* request,
            grpc::AlarmEvent response
        ) override;

        grpc::Status LogManager(
            grpc::ServerContext* context,
            const grpc::LogReq* request,
            grpc::LogEvent* response
        ) override;
    private:
};