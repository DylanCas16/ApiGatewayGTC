#include "log_consumer.hpp"
#include <chrono>


LogConsumer::LogConsumer(Registry& registry) : registry_(registry) {};

void LogConsumer::receiveLogMessages(const LOG::RecordList& record_list) {
    int64_t gateway_ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    for (CORBA::ULong i = 0; i<record_list.length(); ++i) {
        const LOG::Record& record = record_list[i];
        gateway::LogEvent log_event = LogAdapter::fromRecord(record, gateway_ts);
        
        registry_.dispatch(
            Registry::recordKey(record.gcs_component.in()),
            log_event
        );
    }
}