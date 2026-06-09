#include "corba_servant.hpp"
#include <iostream>
#include <string>


CORBA::Object_var CorbaServant::activateServantObject(PortableServer::ServantBase* impl) {
    PortableServer::ObjectId_var oid = poa_->activate_object(impl);
    return poa_->id_to_reference(oid);
}

void CorbaServant::deactivateServant(PortableServer::ServantBase*& impl, const char* name) {
    if (!impl) return;
    try {
        PortableServer::ObjectId_var oid = poa_->servant_to_id(impl);
        poa_->deactivate_object(oid);
    } catch (const CORBA::Exception& e) {
        std::cerr << "[CorbaServant] deactivation (" << name << ") failed: "
                  << e._name() << "\n";
    }
    delete impl;
    impl = nullptr;
}

CorbaServant::CorbaServant(CORBA::ORB_ptr orb, 
                            PortableServer::POA_ptr poa, 
                            MonitorRegistry& monitor_registry,
                            AlarmRegistry& alarm_registry):
                            
                            orb_ (CORBA::ORB::_duplicate(orb)),
                            poa_ (PortableServer::POA::_duplicate(poa)),

                            monitor_consumer_impl_ (nullptr),
                            alarm_consumer_impl_ (nullptr), 

                            monitor_registry_ (monitor_registry),
                            alarm_registry_ (alarm_registry)

{
    activateMonitorConsumer();
    activateAlarmConsumer();
}

CorbaServant::~CorbaServant() {
    deactivateServant(
        reinterpret_cast<PortableServer::ServantBase*&>(monitor_consumer_impl_),
        "MonitorConsumer");
    deactivateServant(
        reinterpret_cast<PortableServer::ServantBase*&>(alarm_consumer_impl_),
        "AlarmConsumer");
}

MM::Consumer_ifce_ptr CorbaServant::getMonitorConsumerObject() const {
    return MM::Consumer_ifce::_duplicate(monitor_consumer_ref_.in());
}

void CorbaServant::activateMonitorConsumer() {
    monitor_consumer_impl_ = new MonitorConsumer(monitor_registry_);
    CORBA::Object_var obj   = activateServantObject(monitor_consumer_impl_);
    monitor_consumer_ref_   = MM::Consumer_ifce::_narrow(obj.in());
}

void CorbaServant::activateAlarmConsumer() {
    alarm_consumer_impl_ = new AlarmConsumer(alarm_registry_);
    CORBA::Object_var obj  = activateServantObject(alarm_consumer_impl_);
    alarm_consumer_ref_    = ALARM::Consumer_ifce::_narrow(obj.in());
}
