#include "log_adapter.hpp"

gateway::LogEvent LogAdapter::fromRecord(const LOG::Record& record) {
    gateway::LogEvent proto_record;

    proto_record.set_component_name(record.gcs_component.in());
    proto_record.set_host_ip_address(static_cast<int32_t>(record.host_ip_address));
    proto_record.set_source_code(record.source.in());
    proto_record.set_type(fromType(record.type));
    proto_record.set_time_stamp(fromTimeValue(record.time_stamp));
    proto_record.set_msg_data(record.msg_data.in());

    return proto_record;
}

gateway::MessageType LogAdapter::fromType(LOG::MessageType type)
{
    switch (type) {
        case LOG::DEBUG:      return gateway::DEBUG;
        case LOG::INFO:       return gateway::INFO;
        case LOG::ERROR_TYPE: return gateway::ERROR_TYPE;
        default:              return gateway::LOG_DOESNT_CARE;
    }
}

int64_t LogAdapter::fromTimeValue(const DGT::TimeValue& time_value)
{
    return static_cast<int64_t>(time_value.usec) * 1000LL;
}
