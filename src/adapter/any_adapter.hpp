#pragma once

#include <tao/ORB.h>
#include <tao/AnyTypeCode/TypeCode.h>
#include "adapter.pb.h"


namespace AnyAdapter {
    gateway::AnyValue fromCorba(const CORBA::Any& any, CORBA::TypeCode_ptr tc);
    CORBA::Any toCorba(const gateway::AnyValue& value);
}