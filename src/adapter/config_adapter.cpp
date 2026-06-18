#include "config_adapter.hpp"


gateway::ConfigEvent ConfigAdapter::fromProperty(const CONFIG::PropertyChange& property_change) {
    gateway::ConfigEvent proto_property;

    proto_property.set_component_name(property_change.gcs_component.in());
    proto_property.set_property_name(property_change.property_name.in());
    proto_property.set_time_stamp(fromTimeValue(property_change.time_stamp));

    return proto_property;
}

int64_t ConfigAdapter::fromTimeValue(const DGT::TimeValue& time_value)
{
    return static_cast<int64_t>(time_value.usec) * 1000LL;
}
