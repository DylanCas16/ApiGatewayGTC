#pragma once

#include <tao/orb.h>
#include "MMCommonC.h"
#include "DGTCommonC.h"
#include "adapter.pb.h"
#include "stream.pb.h"


class MonitorAdapter {
    public:
        static gateway::DataBlocksEvent 
            fromDataBlock(const MM::DataBlock& data);
        
        static gateway::StateChangeEvent 
            fromStateChange(const MM::StateChange& data);
        
        static gateway::MagnitudeChangeEvent 
            fromMagnitudeChange(const MM::MagnitudeChange& data);
        
        static gateway::MultiTypeList
            fromMultiTypeList(const MM::MultiTypeList& list);
    private:
        static void fillSampleList(gateway::SampleList* out,
                                    const MM::SampleDoubleList& in);
        
        static void fillSampleList(gateway::SampleList* out,
                                    const MM::SampleFloatList& in);
            
        static void fillSampleList(gateway::SampleList* out,
                                    const MM::SampleLongList& in);
            
        static void fillSampleList(gateway::SampleList* out,
                                    const MM::SampleShortList& in);
    
        static void fillSampleList(gateway::SampleList* out,
                                    const MM::SampleOctetList& in);
            
        static void fillSampleArrayList(gateway::SampleArrayList* out,
                                        const MM::SampleDoubleArrayList& in);

        static void fillSampleArrayList(gateway::SampleArrayList* out,
                                        const MM::SampleFloatArrayList& in);

        static void fillSampleArrayList(gateway::SampleArrayList* out,
                                        const MM::SampleLongArrayList& in);

        static void fillSampleArrayList(gateway::SampleArrayList* out,
                                        const MM::SampleShortArrayList& in);

        static void fillSampleArrayList(gateway::SampleArrayList* out,
                                        const MM::SampleOctetArrayList& in);
        
        static void fillDoubleArray(gateway::DoubleArray* out, const DGT::DoubleArray& in);
        static void fillFloatArray(gateway::FloatArray* out, const DGT::FloatArray& in);
        static void fillLongArray(gateway::LongArray* out, const DGT::LongArray& in);
        static void fillShortArray(gateway::ShortArray* out, const DGT::ShortArray& in);
}