#include "corba_runtime.hpp"
#include "gcs_endpoints.h"
#include <iostream>
#include <stdexcept>


void CorbaRuntime::init() {
    std::string endpoint = std::string("iiop://") + gcs_env::GCS_DEV_IP + ":0";
    const char* orb_argv[] = { "ApiGatewayGTC", "-ORBListenEndpoints", endpoint.c_str() };
    int argc = 3;
    
    orb_ = CORBA::ORB_init(argc, const_cast<char**>(orb_argv), "");
    if (CORBA::is_nil(orb_.in())) {
        throw std::runtime_error("CorbaRuntime: ORB_init failed");
    }

    CORBA::Object_var poa_obj = orb_->resolve_initial_references("RootPOA");
    root_poa_ = PortableServer::POA::_narrow(poa_obj.in());
    if (CORBA::is_nil(root_poa_.in())) {
        throw std::runtime_error("CorbaRuntime: could not resolve RootPOA");
    }

    PortableServer::POAManager_var mgr = root_poa_->the_POAManager();
    mgr->activate();

    ns_resolver_ = std::unique_ptr<NsResolver>(new NsResolver(orb_.in()));
    ns_resolver_->connect();
 
    ifr_client_ = std::unique_ptr<IfrClient>(new IfrClient(orb_.in()));
    ifr_client_->connect();

    dii_engine_ = std::unique_ptr<DiiEngine>(new DiiEngine(orb_.in()));

    config_service_ = std::unique_ptr<ConfigService>(new ConfigService(*ns_resolver_));
    config_service_->connect();
 
    std::cout << "[CorbaRuntime] init OK" << std::endl;
}

void CorbaRuntime::runInBackground() {
    orb_thread_ = std::unique_ptr<OrbThread>(new OrbThread(orb_.in()));
    if (orb_thread_->activate() != 0)
        throw std::runtime_error("CorbaRuntime: could not activate ORB thread");
 
    std::cout << "[CorbaRuntime] ORB reactor running in background thread" << std::endl;
}
 
void CorbaRuntime::stop() {
    if (!CORBA::is_nil(orb_.in())) {
        orb_->shutdown(true);
        if (orb_thread_)
            orb_thread_->wait();
        orb_->destroy();
    }
    std::cout << "[CorbaRuntime] stopped" << std::endl;
}