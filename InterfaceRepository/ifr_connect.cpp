#include <iostream>
#include <string>
#include <tao/ORB.h>
#include <tao/IFR_Client/IFR_BasicC.h>
#include <orbsvcs/CosNamingC.h>


CORBA::Object_var discoverNs(const CORBA::ORB_ptr orb) {
    CORBA::Object_var ns_obj = orb->resolve_initial_references("NameService");

    CosNaming::NamingContext_var root = CosNaming::NamingContext::_narrow(ns_obj.in());

    CosNaming::Name path;
    path.length(2);
    path[0].id = "Test";
    path[0].kind = "container";
    path[1].id = "InspectorDevice_1";
    path[1].kind = "";

    return root->resolve(path);
}

void printOperations(const CORBA::InterfaceDef::FullInterfaceDescription_var& interface) {
    std::cout << "Interface : " << interface->name.in() << std::endl;
    std::cout << "ID        : " << interface->id.in() << std::endl;
    std::cout << "Operations:" << interface->operations.length() << std::endl;

    for (CORBA::ULong i=0; i < interface->operations.length(); i++) {
        const CORBA::OperationDescription& op = interface->operations[i];

        std::cout << "[" << i << "]" << op.name.in() << std::endl;
        std::cout << "TypeCode: " << op.result->kind();
        
        try { std::cout << " (" << op.result->name() << ")"; }
        catch (...) {}
        std::cout << std::endl;
        
        std::cout << "Parameters: ";
            
        if (op.parameters.length() == 0) {
            std::cout << "(none)";
        } 
        else {
            for (CORBA::ULong p = 0; p < op.parameters.length(); p++) {
                const CORBA::ParameterDescription& param = op.parameters[p];
                const char* type =
                param.mode == CORBA::PARAM_IN    ? "in"    :
                    param.mode == CORBA::PARAM_OUT   ? "out"   : "inout";
            
                std::cout << "\n -  " << type << " " << param.name.in();
            }
        }
        std::cout << "\n\n";
    }
}

void printAttributes(const CORBA::InterfaceDef::FullInterfaceDescription_var& interface) {
    std::cout << "Attributes: " << interface->attributes.length() << std::endl;

    for (CORBA::ULong i = 0; i < interface->attributes.length(); ++i) {
        const CORBA::AttributeDescription& attr = interface->attributes[i];
        std::cout << "  - " << attr.name.in() << "  TypeCode: " << attr.type->kind();
            
        try { std::cout << " (" << attr.type->name() << ")"; }
        catch (...) {}
            
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // Naming Service
        CORBA::Object_var target = discoverNs(orb.in());

        if (CORBA::is_nil(target.in())) {
            std::cerr << "Could not find InspectorDevice_1" << std::endl;
            return 1;
        }
        std::cout << "InspectorDevice_1 found" << std::endl;

        // IFR
        CORBA::Object_var ifr_obj = orb->resolve_initial_references("InterfaceRepository");
        CORBA::Repository_var repository = CORBA::Repository::_narrow(ifr_obj.in());
        if (CORBA::is_nil(repository.in())) {
            std::cerr << "Could not connect to the IFR" << std::endl;
            return 1;
        }
        
        CORBA::String_var repid = target->_repository_id();
        CORBA::Contained_var contained = repository->lookup_id(repid.in());
        if (CORBA::is_nil(contained.in())) {
            std::cerr << "ID not found in the IFR: " << repid.in() << std::endl;
            return 1;
        }

        // InterfaceDef: tipo concreto de IFR
        CORBA::InterfaceDef_var interface_def = CORBA::InterfaceDef::_narrow(contained.in());
        if (is_nil(interface_def.in())) {
            std::cerr << "The Interface found was not a InterfaceDef" << std::endl;
            return 1;
        } 

        // describe_interface(): obtencion de operaciones y atributos
        CORBA::InterfaceDef::FullInterfaceDescription_var interface = 
            interface_def->describe_interface();
        
        printOperations(interface);
        printAttributes(interface);

        orb->destroy();
    } 
    catch (const CORBA::Exception& ex) {
        std::cerr << "CORBA exception: " << ex._info().c_str() << std::endl;
        return 1;
    }
    return 0;
}
