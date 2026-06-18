#pragma once

#include <tao/ORB.h>
#include "LOGC.h"
#include "DGTC.h"
#include "adapter.pb.h"
#include "stream.pb.h"


class LogAdapter {
    public:
        static gateway::LogEvent fromRecord(const LOG::Record& record,
                                            const int64_t gateway_ts);

    private:
        static gateway::MessageType fromType(LOG::MessageType type);
        static int64_t fromTimeValue (const DGT::TimeValue& time_value);
};