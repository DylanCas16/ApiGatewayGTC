#pragma once

#include <tao/ORB.h>
#include "CONFIGC.h"
#include "DGTC.h"
#include "adapter.pb.h"
#include "stream.pb.h"


class ConfigAdapter {
    public:
        static gateway::ConfigEvent fromProperty(const CONFIG::PropertyChange& property_change);
    
    private:
        static int64_t fromTimeValue(const DGT::TimeValue& time_value);
};
