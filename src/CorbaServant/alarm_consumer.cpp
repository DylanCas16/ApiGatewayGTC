#include "alarm_consumer.hpp"


AlarmConsumer::AlarmConsumer(Registry& registry) : registry_(registry) {};

void AlarmConsumer::receiveAlarms(const ALARM::EventList& event_list) {
    for (CORBA::ULong i = 0; i<event_list.length(); ++i) {
        const ALARM::Event& event = event_list[i];

        gateway::AlarmEvent alarm_event;
        *alarm_event.mutable_event() = AlarmAdapter::fromEvent(event);
        
        registry_.dispatch(
            Registry::EventKey(event.name.in(), event.gcs_component.in()),
            alarm_event
        );
    }
}