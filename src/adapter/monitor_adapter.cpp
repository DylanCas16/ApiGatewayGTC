#include "monitor_adapter.hpp"
#include <iostream>


gateway::DataBlocksEvent MonitorAdapter::fromDataBlock(const MM::DataBlock& data) {
    gateway::DataBlocksEvent event;
    
    event.set_id(static_cast<int64_t>(data.id));
    event.set_component_name(data.gcs_component.in());
    event.set_magnitude(data.magnitude.in());
    event.set_time_stamp(static_cast<int64_t>(data.time_stamp.usec));
    *event.mutable_samples(data.samples);
    
    return event;
}

gateway::StateChangeEvent MonitorAdapter::fromStateChange(const MM::StateChange& data) {
    gateway::StateChangeEvent event;
    
    event.set_component_name(data.gcs_component.in());
    event.set_time_stamp(static_cast<int64_t>(data.time_stamp.usec));
    event.set_new_state(data.new_state.scoped_name.in());
    
    return event;
}

gateway::MagnitudeChangeEvent MonitorAdapter::fromMagnitudeChange(const MM::MagnitudeChange& data) {
    gateway::MagnitudeChangeEvent event;
    
    event.set_component_name(data.gcs_component.in());
    event.set_magnitude(data.magnitude.in());
    event.set_time_stamp(static_cast<int64_t>(data.time_stamp.usec));
    event.set_new_value(static_cast<int32_t>(data.new_value));

    return event;
}

gateway::MultiTypeList MonitorAdapter::fromMultiTypeList(const MM::MultiTypeList& list) {
    gateway::MultiTypeList out;

    switch(list._d()) {
        case MM::TYPE_SAMPLE_DOUBLE_LIST:
            fillSampleList(out.mutable_double_list(), list.d());
            break;
        
        case MM::TYPE_SAMPLE_FLOAT_LIST:
            fillSampleList(out.mutable_float_list(), list.f());
            break;
        
        case MM::TYPE_SAMPLE_LONG_LIST:
            fillSampleList(out.mutable_long_list(), list.l());
            break;

        case MM::TYPE_SAMPLE_SHORT_LIST:
            fillSampleList(out.mutable_short_list(), list.s());
            break;

        case MM::TYPE_SAMPLE_OCTET_LIST:
            fillSampleList(out.mutable_octet_list(), list.o());
            break;
        
        case MM::TYPE_SAMPLE_DOUBLE_ARRAY1D_LIST:
        case MM::TYPE_SAMPLE_DOUBLE_ARRAY2D_LIST:
            fillSampleArrayList(out.mutable_double_array_list(), list.array_d());
            break;
        
        case MM::TYPE_SAMPLE_FLOAT_ARRAY1D_LIST:
        case MM::TYPE_SAMPLE_FLOAT_ARRAY2D_LIST:
            fillSampleArrayList(out.mutable_float_array_list(), list.array_f());
            break;

        case MM::TYPE_SAMPLE_LONG_ARRAY1D_LIST:
        case MM::TYPE_SAMPLE_LONG_ARRAY2D_LIST:
            fillSampleArrayList(out.mutable_long_array_list(), list.array_l());
            break;

        case MM::TYPE_SAMPLE_SHORT_ARRAY1D_LIST:
        case MM::TYPE_SAMPLE_SHORT_ARRAY2D_LIST:
            fillSampleArrayList(out.mutable_short_array_list(), list.array_s());
            break;

        case MM::TYPE_SAMPLE_OCTET_ARRAY1D_LIST:
        case MM::TYPE_SAMPLE_OCTET_ARRAY2D_LIST:
            fillSampleArrayList(out.mutable_octet_array_list(), list.array_o());
            break;
        
        default:
            std::cerr << "[MonitorAdapter] MultiTypeList: unknown type "
                << list._d() << std::endl;
    }

    return out;
}
