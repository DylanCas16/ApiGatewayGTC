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
        throw std::runtime_error("NsResolver: not connected - call connect() first");
 
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

std::vector<NsEntry> NsResolver::list(CORBA::ULong max_entries) const {
    std::vector<NsEntry> entries;
    if (CORBA::is_nil(root_.in())) {
        std::cerr << "[NsResolver] not connected" << std::endl;
        return entries;
    }
    listContext(root_.in(), "", max_entries, entries);
    return entries;
}

void NsResolver::listContext(CosNaming::NamingContext_ptr context,
                             const std::string& prefix,
                             CORBA::ULong max_entries,
                             std::vector<NsEntry>& entries) const
{
    CosNaming::BindingList_var bl;
    CosNaming::BindingIterator_var bi;
    context->list(max_entries, bl.out(), bi.out());

    for (CORBA::ULong i = 0; i < bl->length(); ++i) {
        processBinding(context, bl[i], prefix, max_entries, entries);
    }

    if (!CORBA::is_nil(bi.in())) {
        CosNaming::BindingList_var more;
        while (bi->next_n(max_entries, more) && more->length() > 0) {
            for (CORBA::ULong i = 0; i < more->length(); ++i) {
                processBinding(context, more[i], prefix, max_entries, entries);
            }
        }
        bi->destroy();
    }
}

void NsResolver::processBinding(CosNaming::NamingContext_ptr context,
                                const CosNaming::Binding& binding,
                                const std::string& prefix,
                                CORBA::ULong max_entries,
                                std::vector<NsEntry>& entries) const
{
    NsEntry entry;
    entry.name       = binding.binding_name[0].id.in();
    entry.kind       = binding.binding_name[0].kind.in();
    entry.is_context = (binding.binding_type == CosNaming::ncontext);
    entry.path       = prefix.empty() ? entry.name : prefix + "/" + entry.name;

    CosNaming::Name name;
    name.length(1);
    name[0] = binding.binding_name[0];

    try {
        CORBA::Object_var obj = context->resolve(name);
        CORBA::String_var ior = orb_->object_to_string(obj.in());
        entry.ior = ior.in();
        entries.push_back(entry);

        if (entry.is_context) {
            CosNaming::NamingContext_var sub_context =
                CosNaming::NamingContext::_narrow(obj.in());
            if (!CORBA::is_nil(sub_context.in())) {
                listContext(sub_context.in(), entry.path, max_entries, entries);
            }
        }
    } catch (const CORBA::Exception& ex) {
        std::cout << "      ERROR resolving '" << entry.path
                   << "': " << ex._name() << std::endl;
        entries.push_back(entry);
    }
}
