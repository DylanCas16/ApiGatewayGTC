#include "stream_server.hpp"
#include "monitor_adapter.hpp"
#include <vector>
#include <string>
#include <stdio>


grpc::Status Stream::MonitorManager(
    grpc::ServerContext* context,
    const grpc::MonitorReq* request,
    grpc::MonitorEvent* response
)
{
    
    return grpc::Status::OK;
}

grpc::Status Stream::AlarmManager(
    grpc::ServerContext* context,
    const grpc::AlarmReq* request,
    grpc::AlarmEvent* response
)
{
    std::cout << "Alarm subscription in proccess" << std::endl;
    return grpc::Status::OK;
}

grpc::Status Stream::LogManager(
    grpc::ServerContext* context,
    const grpc::LogReq* request,
    grpc::LogEvent* response
)
{
    std::cout << "Log subscription in proccess" << std::endl;
    return grpc::Status::OK;
}
