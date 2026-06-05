#include "monitor_propagator.hpp"
#include "../corba/NamingService/ns_discover.hpp"
#include "../corba/gcs_endpoints.h"
#include <iostream>
#include <stdexcept>


MonitorPropagator::MonitorPropagator(NsResolver& ns, Registry& registry) : ns_(ns), registry_(registry) {}

uint64_t MonitorPropagator::subscribe(const std::string& component, const std::string& magnitude,
                                        gateway::MonitorType type, MM::Consumer_ifce_ptr consumer,
                                        grpc::ServerWriter<gateway::MonitorEvent>* writer) 
{
    ensureConnected();

    std::string key;
    switch (type) {
        case gateway::DataBlocks:
            key = Registry::dataBlockKey(component, magnitude);
            break;
        
        case gateway::StateChanges:
            key = Registry::stateChangeKey(component);
            break;
        
        case gateway::MagnitudeChanges:
            key = Registry::magnitudeChangeKey(component, magnitude);
            break;
        
        default:
            throw std::invalid_argument("[MonitorPropagator] MonitorType invalid");
    }

    if (!registry_.hasActiveWriters(key)) {
        corbaSubscribe(component, magnitude, type, consumer);
        std::cout << "[MonitorPropagator] CORBA subscribe new key: " << key << std::endl;
    } else {
        std::cout << "[MonitorPropagator] CORBA subscribe reuse key: " << key << std::endl;
    }
 
    return registry_.addEntry(key, writer);
}

void MonitorPropagator::unsubscribe(uint64_t id, const std::string& component, const std::string& magnitude,
                                    gateway::MonitorType type, MM::Consumer_ifce_ptr consumer)
{
    bool last = registry_.removeEntry(id);

    if (last) {
        try {
            ensureConnected();
            corbaUnsubscribe(component, magnitude, type, consumer);

            std::cout << "[MonitorPropagator] CORBA unsubscribe: " << component << "/" << magnitude << std::endl;
        } catch (CORBA::Exception& exception) {
            std::cerr << "[MonitorPropagator] CORBA unsubscribe failed: " << exception._name() << std::endl;
        }
    }
}

void MonitorPropagator::ensureConnected() {
    if (isConnected()) return;

    CORBA::Object_var obj = ns_.resolve(gcs_env::GCS_MONITOR_NAME);
    if (CORBA::is_nil(obj.in())) {
        throw std::runtime_error("[MonitorPropagator] MonitorManager Naming Service not found");
    }

    propagator_ = MM::MonitorPropagator_ifce::_narrow(obj.in());
    if (CORBA::is_nil(propagator_.in())) {
        throw std::runtime_error("[MonitorPropagator] Narrow to MonitorPropagator_ifce failed");
    }

    std::cout << "[MonitorPropagator] Connected to MonitorManager" << std::endl;
}

void MonitorPropagator::corbaSubscribe(const std::string& component, const std::string& magnitude,
                                        gateway::MonitorType type, MM::Consumer_ifce_ptr consumer)
{
    switch (type) {
        case gateway::DataBlocks:
            propagator_->subscribeToDataBlocks(component.c_str(), magnitude.c_str(), consumer);
            break;

        case gateway::StateChanges:
            propagator_->subscribeToStateChanges(component.c_str(), consumer);
            break;

        case gateway::MagnitudeChanges:
            propagator_->subscribeToMagnitudeChanges(component.c_str(), magnitude.c_str(), consumer);
            break;

        default:
            break;
    }
}

void MonitorPropagator::corbaUnsubscribe(const std::string& component, const std::string& magnitude,
                                        gateway::MonitorType type, MM::Consumer_ifce_ptr consumer)
{
    switch (type) {
        case gateway::DataBlocks:
            propagator_->unsubscribeToDataBlocks(component.c_str(), magnitude.c_str(), consumer);
            break;
 
        case gateway::StateChanges:
            propagator_->unsubscribeToStateChanges(component.c_str(), consumer);
            break;
 
        case gateway::MagnitudeChanges:
            propagator_->unsubscribeToMagnitudeChanges(component.c_str(), magnitude.c_str(), consumer);
            break;
 
        default:
            break;
    }
}
