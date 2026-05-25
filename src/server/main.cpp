#include "corba/corba_runtime.hpp"
#include "grpc/server.hpp"
#include <grpcpp/grpcpp.h>

int main() {
    CorbaRuntime corba;
    corba.init();
    corba.runInBackground();

    Unary service(corba);

    grpc::ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    auto server = builder.BuildAndStart();

    server->Wait();

    server->Shutdown();
    corba.stop();
    return 0;
}