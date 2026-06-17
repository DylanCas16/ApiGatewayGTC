#include "config_consumer.hpp"


ConfigConsumer::ConfigConsumer(Registry& registry) : registry_(registry) {};

void ConfigConsumer::receiveProperty(const CONFIG::PropertyList& property_list) {
    for (CORBA::ULong i = 0; i<property_list.length(); ++i) {
        const CONFIG::Property& property = property_list[i];
        gateway::ConfigEvent gateway_property = ConfigAdapter::fromProperty(property);
        
        registry_.dispatch(
            Registry::propertyKey(property.name.in()),
            gateway_property
        );
    }
}