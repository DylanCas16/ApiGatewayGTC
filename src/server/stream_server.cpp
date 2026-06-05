#include "stream_server.hpp"
#include "monitor_adapter.hpp"
#include "CorbaServant/corba_servant.hpp"
#include "SubscriptionPropagator/monitor_propagator.hpp"
#include <vector>
#include <string>
#include <stdio>


grpc::Status Stream::MonitorManager(
    grpc::ServerContext* context,
    const grpc::MonitorReq* request,
    grpc::ServerWriter<gateway::MonitorEvent>* writer
)
{
    const std::string& component_name = request->component_name();
    const std::string& magnitude = request->magnitude();
    gateway::MonitorType& type = request->type();


    MM::Consumer_ifce_var consumer = corba_servant_.getMonitorConsumerObject();

    uint64_t id = monitor_propagator_.subscribe(
        component_name, magnitude, type, consumer.in(), writer);
    
    while (!context->IsCancelled() && monitor_registry_.isActive(id)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    monitor_propagator_.unsubscribe(id, component_name, magnitude, type, consumer.in());

    return grpc::Status::OK;
}

grpc::Status Stream::AlarmManager(
    grpc::ServerContext* context,
    const grpc::AlarmReq* request,
    grpc::ServerWriter<gateway::AlarmEvent>* writer
)
{
    std::cout << "Alarm subscription in proccess" << std::endl;
    return grpc::Status::OK;
}

grpc::Status Stream::LogManager(
    grpc::ServerContext* context,
    const grpc::LogReq* request,
    grpc::ServerWriter<gateway::LogEvent>* writer
)
{
    std::cout << "Log subscription in proccess" << std::endl;
    return grpc::Status::OK;
}
