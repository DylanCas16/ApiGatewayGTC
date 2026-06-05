#include "corba_servant.hpp"
#include <iostream>
#include <string>


CorbaServant::CorbaServant(CORBA::ORB_ptr orb, 
                            PortableServer::POA_ptr poa, 
                            MonitorRegistry& registry):
                            
                            orb_ (CORBA::ORB::_duplicate(orb)),
                            poa_ (PortableServer::POA::_duplicate(poa)),
                            monitor_consumer_impl_ (nullptr),
                            monitor_registry_ (registry) 
{
    activateMonitorConsumer();
}

CorbaServant::~CorbaServant() {
    if (monitor_consumer_impl_) {
        try {
            PortableServer::ObjectId_var object_id = 
                poa_->servant_to_id(monitor_consumer_impl_);
            
            poa_->deactivate_object(object_id);
        } catch (const CORBA::Exception& exception) {
            std::cerr << "[CorbaServant] deactivation (MonitorConsumer) failed: " 
                << exception._name() << std::endl;
        }
        delete monitor_consumer_impl_;
        monitor_consumer_impl_ = nullptr;
    }
}

MM::Consumer_ifce_ptr CorbaServant::getMonitorConsumerObject() const {
    return MM::Consumer_ifce::_duplicate(monitor_consumer_ref_.in());
}

void CorbaServant::activateMonitorConsumer() {
    monitor_consumer_impl_ = new MonitorConsumer(monitor_registry_);
    PortableServer::ObjectId_var object_id = poa_->activate_object(monitor_consumer_impl_);

    CORBA::Object_var obj = poa_->id_to_reference(object_id);
    monitor_consumer_ref_ = MM::Consumer_ifce::_narrow(obj.in());
}