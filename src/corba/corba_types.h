#pragma once

#include <tao/ORB.h>
#include <tao/AnyTypeCode/TypeCode.h>
#include <string>
#include <vector>


struct NsEntry {
    std::string name;
    std::string kind;
    std::string ior;
    bool is_context;
};

struct ParamInfo {
    std::string name;
    CORBA::TypeCode_var tc;
    enum Mode { IN, OUT, INOUT, UNSPECIFIED } mode;
};

struct OperationInfo {
    std::string name;
    CORBA::TypeCode_var return_tc;
    std::vector<ParamInfo> params;
};

struct InterfaceInfo {
    std::string repid;
    std::vector<OperationInfo> operations;
};