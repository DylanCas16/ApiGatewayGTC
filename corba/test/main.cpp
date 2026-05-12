#include "corba_runtime.hpp"
#include "NamingService/ns_discover.hpp"
#include "InterfaceRepository/ifr_connect.hpp"

#include <iostream>


static const char* NS_PATH = "Test/InspectorDevice_1";
static const char* OP_NAME = "ping";

int main(int argc, char* argv[]) {
    try {
        CorbaRuntime runtime;
        runtime.init();

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

        std::cout << "IFR test\n";

        std::cout << "\n\n";

        std::cout << "IFR describeInterface\n";
        const InterfaceInfo& info = runtime.ifr().describeInterface(repid.in());
        std::cout << "  operations : " << info.operations.size() << std::endl;
        for (const auto& op : info.operations) {
            std::cout << "    [op] " << op.name
                      << " -> TCKind=" << op.return_tc->kind();
            try { std::cout << " (" << op.return_tc->name() << ")"; }
            catch (...) {}
            std::cout << std::endl;
 
            for (CORBA::ULong p = 0; p < op.params.length(); ++p) {
                const char* mode =
                    op.params[p].mode == CORBA::PARAM_IN    ? "in"    :
                    op.params[p].mode == CORBA::PARAM_OUT   ? "out"   : "inout";
                std::cout << "      - " << mode
                          << " " << op.params[p].name.in() << std::endl;
            }
        }

        std::cout << "\n\n";

        std::cout << "IFR describeInterface (cache)\n";
        const InterfaceInfo& cached = runtime.ifr().describeInterface(repid.in());
        std::cout << "  ops with cache: " << cached.operations.size() << std::endl;

        std::cout << "\n\n";

        std::cout << "IFR preloadAll\n";
        runtime.ifr().preloadAll();

        std::cout << "\n\n";
 
        std::cout << "IFR returnTypeCode (" << OP_NAME << ")\n";
        CORBA::TypeCode_var tc = runtime.ifr().returnTypeCode(repid.in(), OP_NAME);
        std::cout << "TCKind: " << tc->kind() << std::endl;
        try { std::cout << "name: " << tc->name() << std::endl; }
        catch (...) { std::cout << "name: (void)" << std::endl; }

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