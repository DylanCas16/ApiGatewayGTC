#include "corba_runtime.hpp"
#include "NamingService/ns_discover.hpp"
#include "InterfaceRepository/ifr_connect.hpp"
#include "DII/dii_invocation.hpp"

#include <iostream>


static const char* NS_PATH = "Test/InspectorDevice_1";
static const char* OP_NAME = "ping";
static const char* DII_OPS[] = {"voidcommand", "processFloat", "processString"};
static const size_t DII_OPS_COUNT = 3;

std::string testNs(CorbaRuntime& runtime) {
    std::cout << "NamingService test\n";

    std::cout << "\n\n";

    std::cout << "NamingService listRoot\n";
    runtime.ns().listRoot();

    std::cout << "\n\n";

    std::cout << "NamingService resolve\n";
    CORBA::Object_var obj = runtime.ns().resolve(NS_PATH);
    CORBA::String_var repid = obj->_repository_id();
    std::cout << "  path    : " << NS_PATH << std::endl;
    std::cout << "  repo_id : " << repid.in() << std::endl;

    std::cout << "\n\n";

    return std::string(repid.in());
}

void testIfr(CorbaRuntime& runtime, const std::string& repid) {
    std::cout << "IFR test\n";

    std::cout << "\n\n";

    std::cout << "IFR describeInterface\n";
    const InterfaceInfo& info = runtime.ifr().describeInterface(repid);
    std::cout << "  operations : " << info.operations.size() << std::endl;
    for (const auto& op : info.operations) {
        std::cout << "    [op] " << op.name
                  << " -> TCKind=" << op.return_tc->kind();
        try { std::cout << " (" << op.return_tc->name() << ")"; }
        catch (...) {}
        std::cout << std::endl;
 
        for (size_t p = 0; p < op.params.size(); ++p) {
            const char* mode =
                op.params[p].mode == ParamInfo::IN    ? "in"    :
                op.params[p].mode == ParamInfo::OUT   ? "out"   : "inout";
            std::cout << "      - " << mode
                      << " " << op.params[p].name.c_str() << std::endl;
        }
    }
    
    std::cout << "\n\n";
    
    std::cout << "IFR describeInterface (cache)\n";
    const InterfaceInfo& cached = runtime.ifr().describeInterface(repid);
    std::cout << "  ops with cache: " << cached.operations.size() << std::endl;
    std::cout << "\n\n";
    std::cout << "IFR preloadAll\n";
     runtime.ifr().preloadAll();

    std::cout << "\n\n";
 
    std::cout << "IFR returnTypeCode (" << OP_NAME << ")\n";
    CORBA::TypeCode_var tc = runtime.ifr().returnTypeCode(repid, OP_NAME);
    std::cout << "TCKind: " << tc->kind() << std::endl;
    try { std::cout << "name: " << tc->name() << std::endl; }
    catch (...) { std::cout << "name: (void)" << std::endl; }
}

void testDii(CorbaRuntime& runtime, const std::string& repid) {
    std::cout << "DII test\n";
    std::cout << "\n\n";

    CORBA::Object_var target = runtime.ns().resolve(NS_PATH);
    const InterfaceInfo& info = runtime.ifr().describeInterface(repid);

    for (size_t i = 0; i < DII_OPS_COUNT; ++i) {
        const char* op_name = DII_OPS[i];
        std::cout << "DII invoke: " << op_name << "\n";

        const OperationInfo* op_info = NULL;
        for (size_t j = 0; j < info.operations.size(); ++j) {
            if (info.operations[j].name == op_name) {
                op_info = &info.operations[j];
                break;
            }
        }
        if (!op_info) {
            std::cerr << "  operacion no encontrada: " << op_name << std::endl;
            continue;
        }

        std::vector<CORBA::Any> args;
        if (std::string(op_name) == "processFloat") {
            CORBA::Any arg;
            arg <<= static_cast<CORBA::Float>(3.14f);
            args.push_back(arg);
        } else if (std::string(op_name) == "processString") {
            CORBA::Any arg;
            arg <<= "hello_gcs";
            args.push_back(arg);
        }

        try {
            InvokeResult result = runtime.dii().invoke(
                target.in(),
                op_name,
                op_info->return_tc.in(),
                op_info->params,
                args
            );

            CORBA::TypeCode_var ret_tc = result.return_value.type();
            std::cout << "  ok — TCKind: " << ret_tc->kind() << std::endl;
            try   { std::cout << "  tipo: " << ret_tc->name() << std::endl; }
            catch (...) { std::cout << "  tipo: (void)" << std::endl; }

        } catch (const std::exception& e) {
            std::cerr << "  ERROR: " << e.what() << std::endl;
        }

        std::cout << "\n";
    }
}

int main(int argc, char* argv[]) {
    try {
        CorbaRuntime runtime;
        runtime.init();

        std::string repid = testNs(runtime);

        testIfr(runtime, repid);

        testDii(runtime, repid);

        runtime.stop();

    } catch (const CORBA::Exception& ex) {
        std::cerr << "[test] CORBA: " << ex._info().c_str() << std::endl;
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "[test] Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}