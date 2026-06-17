#include "config_adapter.hpp"


gateway::ConfigEvent fromProperty(const CONFIG::PropertyChange& property_change) {
    gateway::ConfigEvent proto_property;

    proto_property.set_config_name(property_change.config_name.in());
    proto_property.set_class_name(property_change.class_name.in());
    proto_property.set_component_name(property_change.gcs_component.in());

    return proto_property;
}
