#include <iostream>
#include <string>
#include <orbsvcs/CosNamingC.h>
#include <tao/ORB.h>
#include <tao/MProfile.h>
#include <tao/Stub.h>
#include <tao/IIOP_Profile.h>


void object_type(CORBA::Object_ptr target) {
    try {
        CORBA::String_var repo_id = target->_repository_id();
        std::cout << "    type    : " << repo_id.in() << std::endl;
    } catch (...) {
        std::cout << "    type    : (not available)" << std::endl;
    }
}

void object_endpoint(CORBA::Object_ptr target) {
    TAO_Stub* stub = target->_stubobj();
    if (!stub) {
        std::cout << "    endpoint: (stub not available)" << std::endl;
        return;
    }

    TAO_MProfile& mp = stub->base_profiles();
    for (CORBA::ULong p = 0; p < mp.profile_count(); ++p) {
        TAO_Profile* profile = mp.get_profile(p);
        CORBA::String_var str = profile->to_string();
        std::cout << "    endpoint: " << str.in() << std::endl;
    }
}

void nslist(CORBA::ORB_ptr orb, CosNaming::NamingContext_ptr context) {
    // Obtener todos los objetos y margen extra.
    CosNaming::BindingList_var bl;
    CosNaming::BindingIterator_var bi;
    context->list(15, bl.out(), bi.out());

    std::cout << "Root bindings (" << bl->length() << "):" << std::endl;
    
    for (CORBA::ULong i = 0; i < bl->length(); ++i) {
        const CosNaming::Binding& b = bl[i];
        
        const char* id   = b.binding_name[0].id.in();
        const char* kind = b.binding_name[0].kind.in();
        bool is_object = (b.binding_type == CosNaming::nobject);

        // Nombre y tipo (objeto o contexto)
        std::cout << "  - " << id;
        if (kind && *kind) std::cout << "." << kind;
        std::cout << "  [" << (is_object ? "object" : "context") << "]" << std::endl;

        try {
            CosNaming::Name name;
            name.length(1);
            name[0] = b.binding_name[0];

            CORBA::Object_var target = context->resolve(name);

            if (is_object) {
                object_type(target.in());
            }
            object_endpoint(target.in());
            
        } catch (const CORBA::Exception& ex) {
            std::cout << "      ERROR: " << ex._name() << std::endl;
        }

        std::cout << std::endl;
    }
}

int main(int argc, char* argv[])
{
    try {
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        CORBA::Object_var obj = orb->resolve_initial_references("NameService");
        if (CORBA::is_nil(obj.in())) {
            std::cerr << "Could not resolve NameService" << std::endl;
            return 1;
        }

        CosNaming::NamingContext_var root =
            CosNaming::NamingContext::_narrow(obj.in());
        if (CORBA::is_nil(root.in())) {
            std::cerr << "Reference is not a NamingContext" << std::endl;
            return 1;
        }

        nslist(orb.in(), root.in());

        orb->destroy();
    }
    catch (const CORBA::Exception& ex) {
        std::cerr << "CORBA exception: " << ex._info().c_str() << std::endl;
        return 1;
    }
    return 0;
}
