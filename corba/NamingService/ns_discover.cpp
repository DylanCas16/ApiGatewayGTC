#include "ns_discover.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
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

NsResolver::NsResolver(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)) {}

void NsResolver::connect() {
    std::cout << "[NsResolver] connecting to " << gcs_env::NS_URL << std::endl;

    CORBA::Object_var obj = orb->string_to_object(gcs_env::NS_URL);
    if (CORBA::is_nil(obj.in())) {
        std::cerr << "Could not resolve NameService" << std::endl;
        return 1;
    }

    root_ = CosNaming::NamingContext::_narrow(obj.in());
    if (CORBA::is_nil(root.in())) {
        std::cerr << "Reference is not a NamingContext" << std::endl;
        return 1;
    }

    std::cout << "[NsResolver] connected" << std::endl;
}

CORBA::Object_var NsResolver::resolve(const std::string& path,
                                      const std::string& leaf_kind) const {
    if (CORBA::is_nil(root_.in()))
        throw std::runtime_error("NsResolver: not connected — call connect() first");
 
    CosNaming::Name name = buildName(path, leaf_kind);
 
    CORBA::Object_var obj = root_->resolve(name);
    if (CORBA::is_nil(obj.in()))
        throw std::runtime_error("NsResolver: path not found: " + path);
 
    return obj._retn();
}

CosNaming::Name NsResolver::buildName(const std::string& path,
                                      const std::string& leaf_kind) {
    std::vector<std::string> segments;
    std::istringstream ss(path);
    std::string seg;
    while (std::getline(ss, seg, '/'))
        if (!seg.empty()) segments.push_back(seg);
 
    if (segments.empty())
        throw std::invalid_argument("NsResolver: empty path");
 
    CosNaming::Name name;
    name.length(static_cast<CORBA::ULong>(segments.size()));
 
    for (CORBA::ULong i = 0; i < name.length(); ++i) {
        name[i].id   = segments[i].c_str();
        name[i].kind = (i + 1 < name.length()) ? "container" : leaf_kind.c_str();
    }
    return name;
}

void NsResolver::listRoot(CORBA::ULong max_entries) const {
    if (CORBA::is_nil(root_.in())) {
        std::cerr << "[NsResolver] not connected" << std::endl;
        return;
    }

    CosNaming::BindingList_var bl;
    CosNaming::BindingIterator_var bi;
    root_->list(max_entries, bl.out(), bi.out());

    std::cout << "Root bindings (" << bl->length() << "):" << std::endl;
    
    for (CORBA::ULong i = 0; i < bl->length(); ++i) {
        const CosNaming::Binding& b = bl[i];
        const char* id   = b.binding_name[0].id.in();
        const char* kind = b.binding_name[0].kind.in();
        bool is_object = (b.binding_type == CosNaming::nobject);

        std::cout << "  - " << id;
        if (kind && *kind) std::cout << "." << kind;
        std::cout << "  [" << (is_object ? "object" : "context") << "]" << std::endl;

        try {
            CosNaming::Name name;
            name.length(1);
            name[0] = b.binding_name[0];

            CORBA::Object_var obj = root_->resolve(name);

            if (is_object) {
                object_type(obj.in());
            }
            object_endpoint(obj.in());
            
        } catch (const CORBA::Exception& ex) {
            std::cout << "      ERROR: " << ex._name() << std::endl;
        }
        std::cout << std::endl;
    }
}
