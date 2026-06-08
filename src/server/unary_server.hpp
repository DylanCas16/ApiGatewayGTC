#pragma once

#include <grpcpp/grpcpp.h>
#include "server.grpc.pb.h"
#include "corba_runtime.hpp"


class Unary {
    public:
        explicit Unary(CorbaRuntime& corba) : corba_(corba) {}

        grpc::Status Invoke(
            grpc::ServerContext* context,
            const gateway::InvokeRequest* request,
            gateway::InvokeResponse* response
        );

        grpc::Status Resolve(
            grpc::ServerContext* context,
            const gateway::ResolveRequest* request,
            gateway::ComponentInfo* response
        );

        grpc::Status ListNaming(
            grpc::ServerContext* context,
            const gateway::NamingRequest* request,
            gateway::NamingResponse* response
        );
    
    private:
        CorbaRuntime& corba_;
};  