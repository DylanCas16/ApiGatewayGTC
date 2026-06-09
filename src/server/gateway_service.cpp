#include "gateway_service.hpp"
 
// stream_server
 
grpc::Status GatewayService::Invoke(
    grpc::ServerContext*          context,
    const gateway::InvokeRequest* request,
    gateway::InvokeResponse*      response)
{
    return unary_.Invoke(context, request, response);
}
 
grpc::Status GatewayService::Resolve(
    grpc::ServerContext*           context,
    const gateway::ResolveRequest* request,
    gateway::ComponentInfo*        response)
{
    return unary_.Resolve(context, request, response);
}
 
grpc::Status GatewayService::ListNaming(
    grpc::ServerContext*          context,
    const gateway::NamingRequest* request,
    gateway::NamingResponse*      response)
{
    return unary_.ListNaming(context, request, response);
}

// config_server
/*
grpc::Status GetProperty(
    grpc::ServerContext* context,
    const gateway::GetPropRequest* request,
    gateway::PropertyValue* response)
{
    return config_.GetProperty(context, request, response);
}

grpc::Status SetProperty(
    grpc::ServerContext* context,
    const gateway::SetPropRequest* request,
    grpc::SetPropResponse* response)
{
    return config_.SetProperty(context, request, response);
}

    grpc::Status SubscribeConfig(
        grpc::ServerContext* context,
        const gateway::ConfigReq* request,
        grpc::ServerWriter<gateway::ConfigEvent>* writer)
{
    return config_.SubscribeConfig(context, request, writer);
}
*/ 
// stream_server
 
grpc::Status GatewayService::SubscribeMonitor(
    grpc::ServerContext*                       context,
    const gateway::MonitorReq*                 request,
    grpc::ServerWriter<gateway::MonitorEvent>* writer)
{
    return stream_.SubscribeMonitor(context, request, writer);
}
 
grpc::Status GatewayService::SubscribeAlarms(
    grpc::ServerContext*                      context,
    const gateway::AlarmReq*                  request,
    grpc::ServerWriter<gateway::AlarmEvent>*  writer)
{
    return stream_.SubscribeAlarms(context, request, writer);
}
 
grpc::Status GatewayService::SubscribeLogs(
    grpc::ServerContext*                     context,
    const gateway::LogReq*                   request,
    grpc::ServerWriter<gateway::LogEvent>*   writer)
{
    return stream_.SubscribeLogs(context, request, writer);
}