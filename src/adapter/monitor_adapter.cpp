#include "monitor_adapter.hpp"
#include "stream.pb.h"
#include "adapter.pb.h"
#include <iostream>


gateway::DataBlockEvent MonitorAdapter::fromDataBlock(const MM::DataBlock& data) {
    gateway::DataBlockEvent event;
    
    event.set_id(static_cast<int64_t>(data.id));
    event.set_component_name(data.gcs_component.in());
    event.set_magnitude(data.magnitude.in());
    event.set_time_stamp(static_cast<int64_t>(data.time_stamp.usec));
    *event.mutable_samples() = MonitorAdapter::fromMultiTypeList(data.samples);
    
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
            fillSampleArrayList(out.mutable_double_arraylist(), list.array_d());
            break;
        
        case MM::TYPE_SAMPLE_FLOAT_ARRAY1D_LIST:
        case MM::TYPE_SAMPLE_FLOAT_ARRAY2D_LIST:
            fillSampleArrayList(out.mutable_float_arraylist(), list.array_f());
            break;

        case MM::TYPE_SAMPLE_LONG_ARRAY1D_LIST:
        case MM::TYPE_SAMPLE_LONG_ARRAY2D_LIST:
            fillSampleArrayList(out.mutable_long_arraylist(), list.array_l());
            break;

        case MM::TYPE_SAMPLE_SHORT_ARRAY1D_LIST:
        case MM::TYPE_SAMPLE_SHORT_ARRAY2D_LIST:
            fillSampleArrayList(out.mutable_short_arraylist(), list.array_s());
            break;

        case MM::TYPE_SAMPLE_OCTET_ARRAY1D_LIST:
        case MM::TYPE_SAMPLE_OCTET_ARRAY2D_LIST:
            fillSampleArrayList(out.mutable_octet_arraylist(), list.array_o());
            break;
        
        default:
            std::cerr << "[MonitorAdapter] MultiTypeList: unknown type "
                << list._d() << std::endl;
    }

    return out;
}

void MonitorAdapter::fillSampleList(gateway::SampleList* out, const MM::SampleDoubleList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::Sample* sample = out->add_samples();
        sample->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
        sample->set_double_val(in[i].value);
    }
}
 
void MonitorAdapter::fillSampleList(gateway::SampleList* out, const MM::SampleFloatList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::Sample* sample = out->add_samples();
        sample->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
        sample->set_float_val(in[i].value);
    }
}
 
void MonitorAdapter::fillSampleList(gateway::SampleList* out, const MM::SampleLongList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::Sample* sample = out->add_samples();
        sample->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
        sample->set_long_val(static_cast<int32_t>(in[i].value));
    }
}
 
void MonitorAdapter::fillSampleList(gateway::SampleList* out, const MM::SampleShortList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::Sample* sample = out->add_samples();
        sample->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
        sample->set_short_val(static_cast<int32_t>(in[i].value));
    }
}
 
void MonitorAdapter::fillSampleList(gateway::SampleList* out, const MM::SampleOctetList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::Sample* sample = out->add_samples();
        sample->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
        sample->set_octet_val(static_cast<uint32_t>(in[i].value));
    }
}

void MonitorAdapter::fillSampleArrayList(gateway::SampleArrayList* out, const MM::SampleDoubleArrayList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::SampleArray* sample_array = out->add_samples();
        sample_array->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
        fillDoubleArray(sample_array->mutable_double_array(), in[i].value);
    }
}
 
void MonitorAdapter::fillSampleArrayList(gateway::SampleArrayList* out, const MM::SampleFloatArrayList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::SampleArray* sample_array = out->add_samples();
        sample_array->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
        fillFloatArray(sample_array->mutable_float_array(), in[i].value);
    }
}
 
void MonitorAdapter::fillSampleArrayList(gateway::SampleArrayList* out, const MM::SampleLongArrayList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::SampleArray* sample_array = out->add_samples();
        sample_array->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
        fillLongArray(sample_array->mutable_long_array(), in[i].value);
    }
}
 
void MonitorAdapter::fillSampleArrayList(gateway::SampleArrayList* out, const MM::SampleShortArrayList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::SampleArray* sample_array = out->add_samples();
        sample_array->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
        fillShortArray(sample_array->mutable_short_array(), in[i].value);
    }
}
 
void MonitorAdapter::fillSampleArrayList(gateway::SampleArrayList* out, const MM::SampleOctetArrayList& in) {
    for (CORBA::ULong i = 0; i < in.length(); ++i) {
        gateway::SampleArray* sample_array = out->add_samples();
        sample_array->set_time_stamp(static_cast<int64_t>(in[i].time_stamp.usec));
 
        const CORBA::Octet* buffer = in[i].value.data.get_buffer();
        sample_array->set_octet_array(reinterpret_cast<const char*>(buffer),
            static_cast<std::size_t>(in[i].value.data.length())
        );
    }
}

void MonitorAdapter::fillDoubleArray(gateway::DoubleArray* out, const DGT::DoubleArray& in) {
    gateway::Dimension* dim = out->mutable_dimension();
    
    dim->set_rows(static_cast<uint32_t>(in.dimension.height));
    dim->set_cols(static_cast<uint32_t>(in.dimension.width));
    
    for (CORBA::ULong j = 0; j < in.data.length(); ++j) {
        out->add_data(in.data[j]);
    }   
}
 
void MonitorAdapter::fillFloatArray(gateway::FloatArray* out, const DGT::FloatArray& in) {
    gateway::Dimension* dim = out->mutable_dimension();
    
    dim->set_rows(static_cast<uint32_t>(in.dimension.height));
    dim->set_cols(static_cast<uint32_t>(in.dimension.width));
    
    for (CORBA::ULong j = 0; j < in.data.length(); ++j) {
        out->add_data(in.data[j]);
    }
}
 
void MonitorAdapter::fillLongArray(gateway::LongArray* out, const DGT::LongArray& in) {
    gateway::Dimension* dim = out->mutable_dimension();
    
    dim->set_rows(static_cast<uint32_t>(in.dimension.height));
    dim->set_cols(static_cast<uint32_t>(in.dimension.width));
    
    for (CORBA::ULong j = 0; j < in.data.length(); ++j) {
        out->add_data(static_cast<int32_t>(in.data[j]));
    }
}
 
void MonitorAdapter::fillShortArray(gateway::ShortArray* out, const DGT::ShortArray& in) {
    gateway::Dimension* dim = out->mutable_dimension();
    
    dim->set_rows(static_cast<uint32_t>(in.dimension.height));
    dim->set_cols(static_cast<uint32_t>(in.dimension.width));
    
    for (CORBA::ULong j = 0; j < in.data.length(); ++j) {
        out->add_data(static_cast<int32_t>(in.data[j]));
    }
}
