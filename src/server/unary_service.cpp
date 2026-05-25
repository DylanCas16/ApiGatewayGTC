#include "server.hpp"
#include <vector>
#include <string>


grpc::Status Unary::Invoke(
    grpc::ServerContext* context,
    const gateway::InvokeRequest* request,
    gateway::InvokeResponse* response)
{

}

grpc::Status Unary::Resolve(
    grpc::ServerContext* context,
    const gateway::ResolveRequest* request,
    gateway::ComponentInfo* response) 
{
    std::string component_name = request->component_name();

    CORBA::Object_var target = corba_.ns().resolve(component_name);
    const char* repid = target->_repository_id();

    InterfaceInfo interface = corba_.ifr().describeInterface(repid.in());

    response->set_component_name(component_name);

    for (const OperationInfo& operation : interface.operations) {
        gateway::Method* proto_method = response->add_methods();
        proto_method->set_name(operation.name);
        proto_method->set_resulttypecode(AnyAdapter::toTypeKind(operation.return_tc.in()));

        for (const ParamInfo& param : operation.params) {
            gateway::Param* proto_param = method->add_params();
            proto_param->set_name(param.name);
            proto_param->set_typecode(AnyAdapter::toTypeKind(param.tc.in()));
            proto_param->set_mode(static_cast<gateway::Mode>(param.mode));
        }
    }

    return grpc::Status::OK;
}

grpc::Status Unary::ListNaming(
    grpc::ServerContext* context,
    const gateway::NamingRequest* request,
    gateway::NamingResponse* response) 
{
    std::vector<gateway::NamingEntry> entries = corba_.ns().listRoot();
    
    for (const NsEntry& entry : entries) {
        gateway::NamingEntry* proto_entry = response->add_entries();
        
        proto_entry->set_name(
            entry.kind.empty() ? entry.name : entry.name + "." + entry.kind);
        proto_entry->set_ior(entry.ior);
    }

    return grpc::Status::OK;
}
