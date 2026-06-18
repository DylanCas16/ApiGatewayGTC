#include "config_consumer.hpp"
#include <chrono>


ConfigConsumer::ConfigConsumer(Registry& registry) : registry_(registry) {};

void ConfigConsumer::receivePropertyChanges(const CONFIG::PropertyChangeList& property_list) {
    int64_t gateway_ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    for (CORBA::ULong i = 0; i<property_list.length(); ++i) {
        const CONFIG::PropertyChange& property_change = property_list[i];
        gateway::ConfigEvent gateway_property = ConfigAdapter::fromProperty(property_change, gateway_ts);
        
        std::string full_id = std::string(property_change.gcs_component.in())
                            + "."
                            + std::string(property_change.property_name.in());

        registry_.dispatch(Registry::propertyKey(full_id), gateway_property);
    }
}