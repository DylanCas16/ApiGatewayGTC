#pragma once

#include "dii_invocation.hpp"
#include "DCFC.h"
#include <string>
#include <vector>
#include <memory>


class DeviceDispatcher {
    public:
        static bool isDeviceMethod(const std::string& method);

        static std::unique_ptr<InvokeResult> dispatch(
            DCF::Device_ifce_ptr device,
            const std::string& method,
            const std::vector<CORBA::Any>& args
        );    
    private:
        static CORBA::Any makeVoid();
        static CORBA::Any makeBool(CORBA::Boolean b);
        static CORBA::Any makeString(const char* s);
};