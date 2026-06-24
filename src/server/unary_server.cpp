#include "unary_server.hpp"
#include "any_adapter.hpp"
#include "../corba/DII/device_dispatcher.hpp"
#include "DCFC.h"
#include <vector>
#include <string>


grpc::Status Unary::Invoke(
    grpc::ServerContext* context,
    const gateway::InvokeRequest* request,
    gateway::InvokeResponse* response)
{
    const std::string& component_name = request->component_name();
    const std::string& method_name = request->method_name();
    
    std::vector<CORBA::Any> final_args;
    for (gateway::AnyValue arg : request->args()) {
        final_args.push_back(AnyAdapter::toCorba(arg));
    }

    CORBA::Object_var target = corba_.ns().resolve(component_name);

    DCF::Device_ifce_var device = DCF::Device_ifce::_narrow(target);
    if (!CORBA::is_nil(device)) {
        try {
            std::unique_ptr<InvokeResult> device_result =
                DeviceDispatcher::dispatch(device, method_name, final_args);
            if (device_result) {
                CORBA::TypeCode_var return_tc = device_result->return_value.type();
                
                CORBA::TCKind kind = return_tc->kind();
                if (kind != CORBA::tk_null && kind != CORBA::tk_void) {
                    *response->mutable_return_value() =
                        AnyAdapter::fromCorba(device_result->return_value, return_tc.in());
                }

                for (const CORBA::Any& out : device_result->out_args) {
                    CORBA::TypeCode_var tc = out.type();
                    *response->add_out_args() = AnyAdapter::fromCorba(out, tc.in());
                }

                return grpc::Status::OK;
            }
        } catch (const std::exception& exception) {
            return grpc::Status(grpc::StatusCode::INTERNAL, exception.what());
        }
    }

    const char* repid = target->_repository_id();

    const InterfaceInfo info = corba_.ifr().describeInterface(repid);

    const OperationInfo* op_info = nullptr;
    for (const OperationInfo& op : info.operations) {
        if (op.name == method_name) { op_info = &op; break; }
    }
    
    if (!op_info) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "method not found: " + method_name);
    }

    try {
        InvokeResult result = corba_.dii().invoke(
                                                target.in(), 
                                                method_name, 
                                                op_info->return_tc.in(), 
                                                op_info->params, 
                                                final_args
        );

        CORBA::TypeCode_var ret_tc = result.return_value.type();

        *response->mutable_return_value() = 
            AnyAdapter::fromCorba(result.return_value, ret_tc.in());

        for (std::size_t i = 0; i < result.out_args.size(); ++i) {
            CORBA::TypeCode_var tc = result.out_args[i].type();
            *response->add_out_args() =
                AnyAdapter::fromCorba(result.out_args[i], tc.in());
        }

    } catch (const CORBA::Exception& ex) {
        return grpc::Status(grpc::StatusCode::INTERNAL, ex._name());
    } catch (const std::exception& e) {
        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());
    }

    return grpc::Status::OK;
}

grpc::Status Unary::Resolve(
    grpc::ServerContext* context,
    const gateway::ResolveRequest* request,
    gateway::ComponentInfo* response) 
{
    std::string component_name = request->component_name();

    CORBA::Object_var target = corba_.ns().resolve(component_name);
    const char* repid = target->_repository_id();

    InterfaceInfo interface = corba_.ifr().describeInterface(repid);

    response->set_component_name(component_name);

    for (const OperationInfo& operation : interface.operations) {
        gateway::Method* proto_method = response->add_methods();
        proto_method->set_name(operation.name);
        proto_method->set_resulttypecode(AnyAdapter::toTypeKind(operation.return_tc.in()));

        for (const ParamInfo& param : operation.params) {
            gateway::Param* proto_param = proto_method->add_params();
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
    std::vector<NsEntry> entries = corba_.ns().list();
    
    for (const NsEntry& entry : entries) {
        gateway::NamingEntry* proto_entry = response->add_entries();
        
        proto_entry->set_name(
            entry.kind.empty() ? entry.name : entry.name + "." + entry.kind);
        proto_entry->set_path(entry.path);
        proto_entry->set_ior(entry.ior);
    }

    return grpc::Status::OK;
}
