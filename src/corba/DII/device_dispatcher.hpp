#pragma once

#include "dii_invocation.hpp"
#include <string>
#include <vector>
#include <memory>


class DeviceDispatcher {
    public:
        static bool isDeviceMethod(const std::string& method);

        static std::unique_ptr<InvokeResult> tryDispatch(
            CORBA::Object_ptr target,
            const std::string& method,
            const std::vector<CORBA::Any>& args
        );    
    private:
        static CORBA::Any makeVoid();
        static CORBA::Any makeBool(CORBA::Boolean b);
        static CORBA::Any makeString(const char* s);
};