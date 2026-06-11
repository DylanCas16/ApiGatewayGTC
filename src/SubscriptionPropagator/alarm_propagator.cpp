#include "alarm_propagator.hpp"

#include "../corba/gcs_endpoints.h"
#include <iostream>
#include <stdexcept>


AlarmPropagator::AlarmPropagator(NsResolver& ns, Registry& registry) :
    Base(ns, registry, gcs_env::GCS_ALARM_NAME) {}

uint64_t AlarmPropagator::subscribe(const std::string& component,
                            ALARM::Consumer_ifce_ptr consumer,
                            grpc::ServerWriter<gateway::AlarmEvent>* writer)
{
    ensureConnected();

    std::string key = Registry::eventKey(component);
    if (!registry_.hasActiveWriters(key)) {
        corbaSubscribe(consumer);
        std::cout << "[AlarmPropagator] CORBA subscribe new key: " << key << std::endl;
    } else {
        std::cout << "[AlarmPropagator] CORBA subscribe reuse key: " << key << std::endl;
    }
 
    return registry_.addEntry(key, writer);
}

void AlarmPropagator::unsubscribe(uint64_t id, const std::string& component,
                                    ALARM::Consumer_ifce_ptr consumer)
{
    bool last = registry_.removeEntry(id);

    if (last) {
        try {
            ensureConnected();
            corbaUnsubscribe(consumer);

            std::cout << "[AlarmPropagator] CORBA unsubscribe: " << component << std::endl;
        } catch (CORBA::Exception& exception) {
            std::cerr << "[AlarmPropagator] CORBA unsubscribe failed: " << exception._name() << std::endl;
        }
    }
}

void AlarmPropagator::narrowCorbaObject(CORBA::Object_ptr obj)
{
    propagator_ = ALARM::Manager_ifce::_narrow(obj);
    if (CORBA::is_nil(propagator_.in())) {
        throw std::runtime_error("[AlarmPropagator] Narrow to Manager_ifce failed");
    }

    std::cout << "[AlarmPropagator] Connected to AlarmManager\n";
}

void AlarmPropagator::corbaSubscribe(ALARM::Consumer_ifce_ptr consumer)
{
    propagator_->subscribeToAlarms(consumer);
}

void AlarmPropagator::corbaUnsubscribe(ALARM::Consumer_ifce_ptr consumer)
{
    propagator_->unsubscribeToAlarms(consumer);
}
