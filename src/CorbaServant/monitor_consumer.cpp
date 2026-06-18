#include "monitor_consumer.hpp"
#include <chrono>


MonitorConsumer::MonitorConsumer(Registry& registry) : registry_(registry) {}

void MonitorConsumer::receiveDataBlocks(const MM::DataBlockList& data_list) {
    int64_t gateway_ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    for (CORBA::ULong i=0; i<data_list.length(); ++i) {
        const MM::DataBlock& data = data_list[i];

        gateway::MonitorEvent event;
        *event.mutable_data_block() = MonitorAdapter::fromDataBlock(data);
        event.set_gateway_ts(gateway_ts);
        
        registry_.dispatch(
            Registry::dataBlockKey(data.gcs_component.in(), std::string(data.magnitude.in())),
            event
        );
    }
}

void MonitorConsumer::receiveStateChanges(const MM::StateChangeList& state_list) {
    int64_t gateway_ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    for (CORBA::ULong i = 0; i<state_list.length(); ++i) {
        const MM::StateChange& data = state_list[i];

        gateway::MonitorEvent event;
        *event.mutable_state_change() = MonitorAdapter::fromStateChange(data);
        event.set_gateway_ts(gateway_ts);

        registry_.dispatch(
            Registry::stateChangeKey(data.gcs_component.in()),
            event
        );
    }
}

void MonitorConsumer::receiveMagnitudeChanges(const MM::MagnitudeChangeList& magnitude_list) {
    int64_t gateway_ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    for (CORBA::ULong i=0; i<magnitude_list.length(); ++i) {
        const MM::MagnitudeChange data = magnitude_list[i];

        gateway::MonitorEvent event;
        *event.mutable_magnitude_change() = MonitorAdapter::fromMagnitudeChange(data);
        event.set_gateway_ts(gateway_ts);

        registry_.dispatch(
            Registry::magnitudeChangeKey(data.gcs_component.in(), std::string(data.magnitude.in())),
            event
        );
    }
}