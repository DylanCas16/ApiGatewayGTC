#include "alarm_adapter.hpp"

gateway::AlarmEvent AlarmAdapter::fromEvent(const ALARM::Event& event) {
    gateway::AlarmEvent proto_event;

    proto_event.set_alarm_name(event.name.in());
    proto_event.set_component_name(event.gcs_component.in());
    proto_event.set_state(fromState(event.new_state));
    proto_event.set_severity(fromSeverity(event.importance));
    proto_event.set_description(event.description.in());
    proto_event.set_time_stamp(fromTimeValue(event.time_stamp));
    proto_event.set_id(static_cast<int64_t>(event.id));

    return proto_event;
}

gateway::AlarmState AlarmAdapter::fromState(ALARM::State_t state)
{
    switch (state) {
        case ALARM::ACTIVE_NACKED:   return gateway::ACTIVE_NACKED;
        case ALARM::ACTIVE_ACKED:    return gateway::ACTIVE_ACKED;
        case ALARM::INACTIVE_NACKED: return gateway::INACTIVE_NACKED;
        case ALARM::INACTIVE_ACKED:  return gateway::INACTIVE_ACKED;
        case ALARM::SOLVED:          return gateway::SOLVED;
        case ALARM::ACK:             return gateway::ACK;
        default:                     return gateway::STATE_DOESNT_CARE;
    }
}

gateway::AlarmSeverity AlarmAdapter::fromSeverity(ALARM::Severity_t severity)
{
    switch (severity) {
        case ALARM::LEVEL1: return gateway::LEVEL1;
        case ALARM::LEVEL2: return gateway::LEVEL2;
        case ALARM::LEVEL3: return gateway::LEVEL3;
        case ALARM::LEVEL4: return gateway::LEVEL4;
        case ALARM::LEVEL5: return gateway::LEVEL5;
        default:            return gateway::SEVERITY_DOESNT_CARE;
    }
}

int64_t AlarmAdapter::fromTimeValue(const DGT::TimeValue& time_value)
{
    return static_cast<int64_t>(time_value.usec) * 1000LL;
}
