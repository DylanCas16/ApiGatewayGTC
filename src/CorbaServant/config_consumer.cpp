#include "config_consumer.hpp"


ConfigConsumer::ConfigConsumer(Registry& registry) : registry_(registry) {};

void ConfigConsumer::receiveProperty(const CONFIG::PropertyChangeList& property_list) {
    for (CORBA::ULong i = 0; i<property_list.length(); ++i) {
        const CONFIG::PropertyChange& property_change = property_list[i];
        gateway::ConfigEvent gateway_property = ConfigAdapter::fromProperty(property_change);
        
        registry_.dispatch(
            Registry::propertyKey(property_change.name.in()),
            gateway_property
        );
    }
}