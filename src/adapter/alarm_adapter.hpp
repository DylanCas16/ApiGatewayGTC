#pragma once

#include <tao/ORB.h>
#include "ALARMCommonC.h"
#include "DGTC.h"
#include "adapter.pb.h"
#include "stream.pb.h"


class AlarmAdapter {
    public:
        static gateway::AlarmEvent fromEvent(const ALARM::Event& event);

    private:
        static gateway::AlarmState fromState(ALARM::State_t state);
        static gateway::AlarmSeverity fromSeverity(ALARM::Severity_t severity);
        static int64_t fromTimeValue (const DGT::TimeValue& time_value);
};