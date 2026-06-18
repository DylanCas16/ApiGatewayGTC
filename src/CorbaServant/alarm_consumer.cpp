#include "alarm_consumer.hpp"
#include <chrono>


AlarmConsumer::AlarmConsumer(Registry& registry) : registry_(registry) {};

void AlarmConsumer::receiveAlarms(const ALARM::EventList& event_list) {
    int64_t gateway_ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    for (CORBA::ULong i = 0; i<event_list.length(); ++i) {
        const ALARM::Event& event = event_list[i];
        gateway::AlarmEvent alarm_event = AlarmAdapter::fromEvent(event, gateway_ts);
        
        registry_.dispatch(
            Registry::eventKey(event.gcs_component.in()),
            alarm_event
        );
    }
}