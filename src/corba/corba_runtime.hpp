#pragma once

#include "NamingService/ns_discover.hpp"
#include "InterfaceRepository/ifr_connect.hpp"
#include "DII/dii_invocation.hpp"
#include "Config/config_service.hpp"
#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>
#include <ace/Task.h>
#include <memory>


class OrbThread : public ACE_Task_Base {
    public:
        explicit OrbThread(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)) {}
        int svc() override {
            orb_->run();
            return 0;
        }
    private:
        CORBA::ORB_var orb_;
};

class CorbaRuntime {
    public:
        CorbaRuntime() = default;
        ~CorbaRuntime() = default;

        CorbaRuntime(const CorbaRuntime&);
        CorbaRuntime& operator = (const CorbaRuntime&);

        void init();

        void runInBackground();

        void stop();

        CORBA::ORB_ptr orb() const { return orb_.in(); }
        PortableServer::POA_ptr rootPoa() const { return root_poa_.in(); }
        
        NsResolver& ns() const { return *ns_resolver_; }
        IfrClient& ifr() const { return *ifr_client_; }
        DiiEngine& dii() const { return *dii_engine_; }
        ConfigService& config() const { return *config_service_; }

    private:
        CORBA::ORB_var orb_;
        PortableServer::POA_var root_poa_;
        std::unique_ptr<OrbThread> orb_thread_;
        
        std::unique_ptr<NsResolver> ns_resolver_;
        std::unique_ptr<IfrClient> ifr_client_;
        std::unique_ptr<DiiEngine> dii_engine_;
        std::unique_ptr<ConfigService> config_service_;
};