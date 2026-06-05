#include "monitor_consumer.hpp"


MonitorConsumer::MonitorConsumer(Registry& registry) : registry_(registry) {}

void MonitorConsumer::receiveDataBlocks(const MM::DataBlockList& data_list) {
    for (CORBA::ULong i=0; i<data_list.length(); ++i) {
        const MM::DataBlock& data = data_list[i];

        gateway::MonitorEvent event;
        *event.mutable_data_block() = MonitorAdapter::fromDataBlock(data);
        
        registry_.dispatch(
            Registry::dataBlockKey(data.gcs_component.in(), std::string(data.magnitude.in())),
            event
        );
    }
}

void MonitorConsumer::receiveStateChanges(const MM::StateChangeList& state_list) {
    for (CORBA::ULong i = 0; i<state_list.length(); ++i) {
        const MM::StateChange& data = state_list[i];

        gateway::MonitorEvent event;
        *event.mutable_state_change() = MonitorAdapter::fromStateChange(data);

        registry_.dispatch(
            Registry::stateChangeKey(data.gcs_component.in()),
            event
        );
    }
}

void MonitorConsumer::receiveMagnitudeChanges(const MM::MagnitudeChangeList& magnitude_list) {
    for (CORBA::ULong i=0; i<magnitude_list.length(); ++i) {
        const MM::MagnitudeChange data = magnitude_list[i];

        gateway::MonitorEvent event;
        *event.mutable_magnitude_change() = MonitorAdapter::fromMagnitudeChange(data);

        registry_.dispatch(
            Registry::magnitudeChangeKey(data.gcs_component.in(), std::string(data.magnitude.in())),
            event
        );
    }
}