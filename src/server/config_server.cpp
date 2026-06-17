#include "config_server.hpp"
#include "any_adapter.hpp"


Config::Config(CorbaRuntime& corba) : corba_(corba) {}

grpc::Status Config::GetProperty(
    grpc::ServerContext* context,
    const gateway::GetPropRequest* request,
    gateway::PropertyValue* response)
{
    try {
        CONFIG::Property property = 
            corba_.config().getProperty(request->property_name());

        response->mutable_property()->set_name(property.name.in());

        CORBA::TypeCode_var tc = property.value.type();
        *response->mutable_property()->mutable_value() =
            AnyAdapter::fromCorba(property.value, tc.in());

        return grpc::Status::OK;

    } catch (const std::exception& exception) {
        return grpc::Status(grpc::StatusCode::INTERNAL, exception.what());
    }
}

grpc::Status Config::SetProperty(
    grpc::ServerContext* context,
    const gateway::SetPropRequest* request,
    gateway::SetPropResponse* response)
{
    try {
        CONFIG::Property property;
        property.name  = CORBA::string_dup(request->property().name().c_str());
        property.value = AnyAdapter::toCorba(request->property().value());

        const bool done = corba_.config().setProperty(property);
        response->set_done(done);

        return grpc::Status::OK;

    } catch (const std::exception& exception) {
        return grpc::Status(grpc::StatusCode::INTERNAL, exception.what());
    }
}

grpc::Status Config::SubscribeConfig(
    grpc::ServerContext* context,
    const gateway::ConfigReq* request,
    grpc::ServerWriter<gateway::ConfigEvent>* writer)
{
    const std::string& property_name = request->property_name();

    CONFIG::Consumer_ifce_var consumer = corba_servant_.getConfigConsumerObject();

    uint64_t id = config_propagator_.subscribe(
        property_name,consumer.in(), writer);
    
    while (!context->IsCancelled() && config_registry_.isActive(id)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    config_propagator_.unsubscribe(id, property_name, consumer.in());

    return grpc::Status::OK;
}
