#include "config_propagator.hpp"

#include "../corba/gcs_endpoints.h"
#include <iostream>
#include <stdexcept>


ConfigPropagator::ConfigPropagator(NsResolver& ns, Registry& registry) :
    Base(ns, registry, gcs_env::GCS_CONFIG_NAME) {}

uint64_t ConfigPropagator::subscribe(const std::string& property,
                                CONFIG::Consumer_ifce_ptr consumer,
                                grpc::ServerWriter<gateway::ConfigEvent>* writer)
{
    ensureConnected();

    std::string key = Registry::propertyKey(property);

    if (!registry_.hasActiveWriters(key)) {
        corbaSubscribe(consumer);
        std::cout << "[ConfigPropagator] CORBA subscribe new key: " << key << std::endl;
    } else {
        std::cout << "[ConfigPropagator] CORBA subscribe reuse key: " << key << std::endl;
    }

    return registry_.addEntry(key, writer);
}

void ConfigPropagator::unsubscribe(uint64_t id, const std::string property,
                                CONFIG::Consumer_ifce_ptr consumer)
{
    bool last = registry_.removeEntry(id);

    if (last) {
        try {
            ensureConnected();
            corbaUnsubscribe(consumer);

            std::cout << "[ConfigPropagator] CORBA unsubscribe: " << property << std::endl;
        } catch (CORBA::Exception& exception) {
            std::cerr << "[ConfigPropagator] CORBA unsubscribe failed: " << exception._name() << std::endl;
        }
    }
}

void ConfigPropagator::narrowCorbaObject(CORBA::Object_ptr obj)
{
    propagator_ = CONFIG::Manager_ifce::_narrow(obj);
    if (CORBA::is_nil(propagator_.in())) {
        throw std::runtime_error("[ConfigPropagator] Narrow to Manager_ifce failed");
    }

    std::cout << "[ConfigPropagator] Connected to ConfigManager\n";
}

void ConfigPropagator::corbaSubscribe(CONFIG::Consumer_ifce_ptr consumer)
{
    propagator_->subscribeToPropertyChanges(consumer);
}

void ConfigPropagator::corbaUnsubscribe(CONFIG::Consumer_ifce_ptr consumer)
{
    propagator_->unsubscribeToPropertyChanges(consumer);
}
