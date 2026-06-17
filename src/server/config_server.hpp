#pragma once

#include <grpcpp/grpcpp.h>
#include <thread>
#include <chrono>

#include "server.grpc.pb.h"
#include "config.pb.h"

#include "corba_runtime.hpp"
#include "../CorbaServant/corba_servant.hpp"
#include "../SubscriptionPropagator/config_propagator.hpp"


class Config {
    public:
        explicit Config(CorbaRuntime& corba);

        grpc::Status GetProperty(
            grpc::ServerContext* context,
            const gateway::GetPropRequest* request,
            gateway::PropertyValue* response
        );

        grpc::Status SetProperty(
            grpc::ServerContext* context,
            const gateway::SetPropRequest* request,
            gateway::SetPropResponse* response
        );

        grpc::Status SubscribeConfig(
            grpc::ServerContext* context,
            const gateway::ConfigReq* request,
            grpc::ServerWriter<gateway::ConfigEvent>* writer
        );
    private:
        CorbaRuntime& corba_;
};