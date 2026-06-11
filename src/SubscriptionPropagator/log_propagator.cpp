#include "log_propagator.hpp"

#include "../corba/gcs_endpoints.h"
#include <iostream>
#include <stdexcept>


LogPropagator::LogPropagator(NsResolver& ns, Registry& registry) :
    Base(ns, registry, gcs_env::GCS_LOG_NAME) {}

uint64_t LogPropagator::subscribe(const std::string& component,
                            LOG::Consumer_ifce_ptr consumer,
                            grpc::ServerWriter<gateway::AlarmEvent>* writer)
{
    ensureConnected();

    std::string key = Registry::recordKey(component);
    if (!registry_.hasActiveWriters(key)) {
        corbaSubscribe(consumer);
        std::cout << "[LogPropagator] CORBA subscribe new key: " << key << std::endl;
    } else {
        std::cout << "[LogPropagator] CORBA subscribe reuse key: " << key << std::endl;
    }
 
    return registry_.addEntry(key, writer);
}

void LogPropagator::unsubscribe(uint64_t id, const std::string& component,
                                    LOG::Consumer_ifce_ptr consumer)
{
    bool last = registry_.removeEntry(id);

    if (last) {
        try {
            ensureConnected();
            corbaUnsubscribe(consumer);

            std::cout << "[LogPropagator] CORBA unsubscribe: " << component << std::endl;
        } catch (CORBA::Exception& exception) {
            std::cerr << "[LogPropagator] CORBA unsubscribe failed: " << exception._name() << std::endl;
        }
    }
}

void LogPropagator::narrowCorbaObject(CORBA::Object_ptr obj)
{
    propagator_ = LOG::Manager_ifce::_narrow(obj);
    if (CORBA::is_nil(propagator_.in())) {
        throw std::runtime_error("[LogPropagator] Narrow to Manager_ifce failed");
    }

    std::cout << "[LogPropagator] Connected to AlarmManager\n";
}

void LogPropagator::corbaSubscribe(LOG::Consumer_ifce_ptr consumer)
{
    propagator_->subscribeToAlarms(consumer);
}

void LogPropagator::corbaUnsubscribe(LOG::Consumer_ifce_ptr consumer)
{
    propagator_->unsubscribeToAlarms(consumer);
}
