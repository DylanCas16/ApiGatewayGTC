#include "config_consumer.hpp"
#include <iostream>


ConfigConsumer::ConfigConsumer(Registry& registry) : registry_(registry) {};

void ConfigConsumer::receivePropertyChanges(const CONFIG::PropertyChangeList& property_list) {
    for (CORBA::ULong i = 0; i<property_list.length(); ++i) {
        const CONFIG::PropertyChange& property_change = property_list[i];
        gateway::ConfigEvent gateway_property = ConfigAdapter::fromProperty(property_change);
        
        std::string full_id = std::string(property_change.gcs_component.in())
                            + "."
                            + std::string(property_change.property_name.in());

        registry_.dispatch(Registry::propertyKey(full_id), gateway_property);
    }
}