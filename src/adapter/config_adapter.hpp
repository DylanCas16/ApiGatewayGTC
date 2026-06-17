#pragma once

#include <tao/ORB.h>
#include "ALARMC.h"
#include "DGTC.h"
#include "adapter.pb.h"
#include "config.pb.h"


class ConfigAdapter {
    public:
        static gateway::ConfigEvent fromProperty(const CONFIG::PropertyChange& property_change);
};
