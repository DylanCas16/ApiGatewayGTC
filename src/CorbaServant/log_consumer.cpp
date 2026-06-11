#include "log_consumer.hpp"


LogConsumer::LogConsumer(Registry& registry) : registry_(registry) {};

void LogConsumer::receiveLogMessages(const LOG::RecordList& record_list) {
    for (CORBA::ULong i = 0; i<record_list.length(); ++i) {
        const LOG::Record& record = record_list[i];
        gateway::LogEvent log_event = LogAdapter::fromRecord(record);
        
        registry_.dispatch(
            Registry::recordKey(record.gcs_component.in()),
            log_event
        );
    }
}