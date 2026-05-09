#pragma once

#include <tao/ORB.h>
#include <tao/PortableServer/PortableServer.h>
#include <ace/Task.h>
#include <memory>


class NsResolver;
class IfrClient;

class OrbThread : public ACE_Task_Base {
    public:
        explicit OrbThread(CORBA::ORB_ptr orb) : orb_(CORBA::ORB::_duplicate(orb)) {}
        int svc() override {
            orb_->run();
            return 0;
        }
    private:
        CORBA::ORB_var orb_;
}

class CorbaRuntime {
    public:
        CorbaRuntime() = default;
        ~CorbaRuntime() = default;

}