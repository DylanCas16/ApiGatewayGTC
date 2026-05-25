#include "ns_discover.hpp"
#include "../gcs_endpoints.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <tao/MProfile.h>
#include <tao/Stub.h>
#include <tao/IIOP_Profile.h>


NsResolver::NsResolver(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)) {}

void NsResolver::connect() {
    std::cout << "[NsResolver] connecting to " << gcs_env::NS_URL << std::endl;

    CORBA::Object_var obj = orb_->string_to_object(gcs_env::NS_URL);
    if (CORBA::is_nil(obj.in())) {
        throw std::runtime_error("Could not resolve NameService");
    }

    root_ = CosNaming::NamingContext::_narrow(obj.in());
    if (CORBA::is_nil(root_.in())) {
        throw std::runtime_error("Reference is not a NamingContext");
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

std::vector<NsEntry> NsResolver::listRoot(CORBA::ULong max_entries) const {
    std::vector<NsEntry> entries;

    if (CORBA::is_nil(root_.in())) {
        std::cerr << "[NsResolver] not connected" << std::endl;
        return entries;
    }

    CosNaming::BindingList_var bl;
    CosNaming::BindingIterator_var bi;
    root_->list(max_entries, bl.out(), bi.out());
    
    for (CORBA::ULong i = 0; i < bl->length(); ++i) {
        const CosNaming::Binding& b = bl[i];

        NsEntry entry;
        entry.name = b.binding_name[0].id.in();
        entry.kind = b.binding_name[0].kind.in();
        entry.is_context = (b.binding_type == CosNaming::nobject);

        try {
            CosNaming::Name name;
            name.length(1);
            name[0] = b.binding_name[0];

            CORBA::Object_var obj = root_->resolve(name);

            CORBA::String_var ior = orb_->object_to_string(obj.in());
            entry.ior = ior.in();
            
        } catch (const CORBA::Exception& ex) {
            std::cout << "      ERROR: " << ex._name() << std::endl;
        }
        
        entries.push_back(entry);
    }

    return entries;
}
