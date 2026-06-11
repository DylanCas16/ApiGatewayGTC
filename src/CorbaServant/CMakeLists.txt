cmake_minimum_required(VERSION 3.17)

add_library(CorbaServant STATIC
    alarm_consumer.cpp
    monitor_consumer.cpp
    corba_servant.cpp
)

target_include_directories(CorbaServant PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${IDL_GEN_DIR}
    ${CMAKE_BINARY_DIR}/generated/proto
)

target_link_libraries(CorbaServant PUBLIC
    SubscriptionRegistry
    lib_adapter
    lib_idl_stubs
    lib_idl_skeletons
    gRPC::grpc++
    protobuf::libprotobuf
)