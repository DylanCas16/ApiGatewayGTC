#include <iostream>
#include <string>
#include <tao/ORB.h>
#include <tao/DynamicInterface/Request.h>
#include <tao/IFR_Client/IFR_BasicC.h>
#include <orbsvcs/CosNamingC.h>


CORBA::Object_var discoverNs(CORBA::ORB_ptr orb) {
    CORBA::Object_var ns_obj = orb->resolve_initial_references("NameService");
    CosNaming::NamingContext_var root = CosNaming::NamingContext::_narrow(ns_obj.in());

    CosNaming::Name path;
    path.length(2);
    path[0].id = "Test";
    path[0].kind = "container";
    path[1].id = "InspectorDevice_1";
    path[1].kind = ""; 

    CORBA::Object_var target = root->resolve(path);

    if (CORBA::is_nil(target.in())) {
        throw std::runtime_error("InspectorDevice_1 not found");
    }

    std::cout << "InspectorDevice_1 found" << std::endl;
    return target._retn();
}

CORBA::TypeCode_var getReturnTypeCode(CORBA::ORB_ptr orb, const char* repid, const char* op_name) {

    CORBA::Object_var ifr_obj = orb->string_to_object("corbaloc:iiop:1.2@gcs-env:12005/InterfaceRepository");
    CORBA::Repository_var repository = CORBA::Repository::_narrow(ifr_obj.in());

    CORBA::Contained_var contained = repository->lookup_id(repid);
    if (CORBA::is_nil(contained.in())) {
        throw std::runtime_error("ID not found in the IFR");
    }

    CORBA::InterfaceDef_var interface_def = CORBA::InterfaceDef::_narrow(contained.in());
    CORBA::InterfaceDef::FullInterfaceDescription_var interface = interface_def->describe_interface();

    for (CORBA::ULong i=0; i < interface->operations.length(); i++) {
        if (std::string(interface->operations[i].name.in()) == op_name) {
            std::cout << "Operation " << op_name << " found" << std::endl;

            CORBA::TypeCode_var tc = interface->operations[i].result;
            std::cout << "TypeCode: " << tc->kind() << std::endl;

            try { std::cout << " (" << tc->name() << ")"; } 
            catch (...) { std::cout << "(void)"; }
            std::cout << std::endl;

            return tc._retn();
        }
    }

    throw std::runtime_error(std::string("Operation not found: ") + op_name);
}

void invokeOperation(CORBA::Object_ptr target, const char* op_name, CORBA::TypeCode_ptr ret_tc) {
    CORBA::Request_var request = target->_request(op_name);

    request->set_return_type(ret_tc);
    std::cout << "Operation " << op_name << " currently working" << std::endl;

    try {
        request->invoke();
        std::cout << "Invocation executed correctly" << std::endl;
    } catch(const CORBA::Exception& exception) {
        std::cerr << "Error: exception in function. " << exception._info().c_str() << std::endl;
        return;
    }

    const CORBA::Any& result = request->return_value();

    switch (ret_tc->kind()) {
        case CORBA::tk_void:
            std::cout << "Answer: void (ok)" << std::endl;
            break;
        case CORBA::tk_boolean: {
            CORBA::Boolean val;
            result >>= CORBA::Any::to_boolean(val);
            std::cout << "Answer: " << (val ? "true" : "false") << std::endl;
            break;
        }
        case CORBA::tk_long: {
            CORBA::Long val;
            result >>= val;
            std::cout << "Answer: " << val << std::endl;
            break;
        }
        case CORBA::tk_string: {
            const char* val;
            result >>= val;
            std::cout << "Answer: " << val << std::endl;
            break;
        }
        default:
            // Para tipos compuestos (struct, sequence...)
            std::cout << "Answer (TCKind=" << ret_tc->kind() << ")" << std::endl;
            break;
    }
}

int main(int argc, char* argv[]) {
    try {
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        CORBA::Object_var target = discoverNs(orb.in());

        CORBA::String_var repid = target->_repository_id();
        std::cout << "Repository ID: " << repid.in() << std::endl;

        const char* op_name = "ping";

        CORBA::TypeCode_var ret_tc = getReturnTypeCode(orb.in(), repid.in(), op_name);

        invokeOperation(target.in(), op_name, ret_tc);

        orb->destroy();

    } catch (const CORBA::Exception& ex) {
        std::cerr << "CORBA exception: " << ex._info().c_str() << std::endl;
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
