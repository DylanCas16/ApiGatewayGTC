#include "ifr_client.h"
#include "../gcs_endpoints.h"

#include <iostream>
#include <stdexcept>


static InterfaceInfo getInterface(const std::string& repid,
            const CORBA::InterfaceDef::FullInterfaceDescription_var& interface) {
    
    InterfaceInfo i_info;
    i_info.repid = repid;
    
    for (CORBA::ULong i = 0; i < interface->operations.length(); ++i) {
        const CORBA::OperationDescription& op = interface->operations[i];
        OperationInfo o_info;
        o_info.name      = op.name.in();
        o_info.return_tc = CORBA::TypeCode::_duplicate(op.result.in());
        o_info.params    = op.parameters;
        i_info.operations.push_back(std::move(o_info));
    }

    std::cout << "[IfrClient] described " << repid
              << " (" << i_info.operations.size() << " ops)" << std::endl;
    return i_info;
}

std::string IfrClient::cacheKey(const std::string& repid,
                                const std::string& method) {
    return repid + "#" + method;
}

IfrClient::IfrClient(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)) {}

void IfrClient::connect() {
    std::cout << "[IfrClient] connecting to " << gcs_env::IFR_URL << std::endl;

    CORBA::Object_var obj = orb_->string_to_object(gcs_env::IFR_URL);
    if (CORBA::is_nil(obj.in())) {
        throw std::runtime_error("IfrClient: string_to_object returned nil");
    }
        
    repository_ = CORBA::Repository::_narrow(obj.in());
    if (CORBA::is_nil(repository_.in())) {
        throw std::runtime_error("IfrClient: reference is not a CORBA::Repository");
    }

    std::cout << "[IfrClient] connected" << std::endl;
}

InterfaceInfo IfrClient::fetchFromIfr(const std::string& repid) {
    CORBA::Contained_var contained = repository_->lookup_id(repid.c_str());
    if (CORBA::is_nil(contained.in())) {
        throw std::runtime_error("ID not found in the IFR: " + repid);
    }

    CORBA::InterfaceDef_var interface_def = CORBA::InterfaceDef::_narrow(contained.in());
    if (CORBA::is_nil(interface_def.in())) {
        throw std::runtime_error("The Interface found was not a InterfaceDef");
    }

    CORBA::InterfaceDef::FullInterfaceDescription_var interface = 
        interface_def->describe_interface();

    return getInteface(interface);
}
 
const InterfaceInfo& IfrClient::describeInterface(const std::string& repid) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
 
    auto it = interface_cache_.find(repid);
    if (it != interface_cache_.end()) return it->second;
 
    InterfaceInfo info = fetchFromIfr(repid);
    populateCache(info);
    auto result = interface_cache_.emplace(repid, std::move(info));
    return result.first->second;
}
 
CORBA::TypeCode_ptr IfrClient::returnTypeCode(const std::string& repid,
                                              const std::string& method) const {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    const std::string key = cacheKey(repid, method);
    auto it = tc_cache_.find(key);
    if (it == tc_cache_.end())
        throw std::out_of_range("IfrClient: no TypeCode for " + key);
    return it->second.in();
}
 
void IfrClient::preloadAll() {
    CORBA::ContainedSeq_var contents =
        repository_->contents(CORBA::dk_Interface, true);
 
    std::cout << "[IfrClient] preloading " << contents->length()
              << " interfaces..." << std::endl;
 
    for (CORBA::ULong i = 0; i < contents->length(); ++i) {
        try {
            describeInterface(std::string(contents[i]->id()));
        } catch (const std::exception& e) {
            std::cerr << "[IfrClient] preload warning: " << e.what() << std::endl;
        }
    }
 
    std::cout << "[IfrClient] preload complete ("
              << tc_cache_.size() << " TypeCodes cached)" << std::endl;
}

void IfrClient::populateCache(const InterfaceInfo& info) {
    for (const auto& op : info.operations)
        tc_cache_[cacheKey(info.repid, op.name)] = 
            CORBA::TypeCode::_duplicate(op.return_tc.in());
}