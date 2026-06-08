#include "device_dispatcher.hpp"
#include <stdexcept>
#include <unordered_map>
#include <iostream>


using VoidMethod = void (DCF::Device_ifce::*)();
using BoolMethod = CORBA::Boolean (DCF::Device_ifce::*)();

static const std::unordered_map<std::string, VoidMethod> VoidMethodsList = {
    {"enable",     &DCF::Device_ifce::enable    },
    {"disable",    &DCF::Device_ifce::disable   },
    {"start",      &DCF::Device_ifce::start     },
    {"init",       &DCF::Device_ifce::init      },
    {"halt",       &DCF::Device_ifce::halt      },
    {"shutdown",   &DCF::Device_ifce::shutdown  },
    {"restore",    &DCF::Device_ifce::restore   },
    {"abort",      &DCF::Device_ifce::abort     },
    {"reset",      &DCF::Device_ifce::reset     },
    {"test",       &DCF::Device_ifce::test      },
    {"ping",       &DCF::Device_ifce::ping      },
    {"startDebug", &DCF::Device_ifce::startDebug},
    {"stopDebug",  &DCF::Device_ifce::stopDebug }
};

static const std::unordered_map<std::string, BoolMethod> BoolMethodsList = {
    {"isOff",          &DCF::Device_ifce::isOff         },
    {"isStarting",     &DCF::Device_ifce::isStarting    },
    {"isShuttingDown", &DCF::Device_ifce::isShuttingDown},
    {"isOn",           &DCF::Device_ifce::isOn          },
    {"isInitialising", &DCF::Device_ifce::isInitialising},
    {"isHalting",      &DCF::Device_ifce::isHalting     },
    {"isIdle",         &DCF::Device_ifce::isIdle        },
    {"isRunning",      &DCF::Device_ifce::isRunning     },
    {"isDisabled",     &DCF::Device_ifce::isDisabled    },
    {"isFault",        &DCF::Device_ifce::isFault       },
    {"isResetting",    &DCF::Device_ifce::isResetting   },
    {"isReady",        &DCF::Device_ifce::isReady       },
    {"isConfigurable", &DCF::Device_ifce::isConfigurable},
    {"isComposite",    &DCF::Device_ifce::isComposite   },
    {"isIntegrated",   &DCF::Device_ifce::isIntegrated  }
};

bool DeviceDispatcher::isDeviceMethod(const std::string& method) {
    return VoidMethodsList.count(method) || BoolMethodsList.count(method) ||
        method == "configure" || method == "report" || method == "internals";
}

std::unique_ptr<InvokeResult> DeviceDispatcher::dispatch(DCF::Device_ifce_ptr device,
                                                            const std::string& method,
                                                            const std::vector<CORBA::Any>& args)
{
    try {
        if (!isDeviceMethod(method)) return nullptr;

        std::cout << "[DeviceDispatcher] Device method: " << method << std::endl;

        InvokeResult result;

        auto void_method = VoidMethodsList.find(method);
        if (void_method != VoidMethodsList.end()) {
            (device->*(void_method->second))();
            result.return_value = makeVoid();
            return std::unique_ptr<InvokeResult>(new InvokeResult(result));
        }

        auto bool_method = BoolMethodsList.find(method);
        if (bool_method != BoolMethodsList.end()) {
            result.return_value = makeBool((device->*(bool_method->second))());
            return std::unique_ptr<InvokeResult>(new InvokeResult(result));
        }

        if (method == "configure") {
            const char* config_name = "";

            if (!args.empty() && !(args[0] >>= config_name)) {
                throw std::runtime_error(
                    "[DeviceDispatcher] DeviceDispatcher::configure: expected string argument");
            }

            device->configure(config_name);
            result.return_value = makeVoid();
            return std::unique_ptr<InvokeResult>(new InvokeResult(result));
        }

        if (method == "report") {
            // level: SHORT=0 NORMAL=1 FULL=2
            CORBA::Short level = 0; 
            if (!args.empty()) args[0] >>= level;

            CORBA::String_var report_value = device->report(level);
            result.return_value = makeString(report_value.in());
            return std::unique_ptr<InvokeResult>(new InvokeResult(result));
        }

        if (method == "internal") {
            CORBA::String_var info = device->internals();
            result.return_value = makeString(info.in());
            return std::unique_ptr<InvokeResult>(new InvokeResult(result));
        }

        std::cerr << "[DeviceDispatcher] WARNING: unhandled method '"
                  << method << "'\n";
        return nullptr;

    } catch (const CORBA::Exception& exception) {
        throw std::runtime_error("DeviceDispatcher: CORBA exception on '"
            + method + "': " + exception._info().c_str());
    }
}

CORBA::Any DeviceDispatcher::makeVoid() {
    return CORBA::Any();
}

CORBA::Any DeviceDispatcher::makeBool(CORBA::Boolean b) {
    CORBA::Any any;
    any <<= CORBA::Any::from_boolean(b);
    return any;
}

CORBA::Any DeviceDispatcher::makeString(const char* s) {
    CORBA::Any any;
    any <<= s;
    return any;
}
