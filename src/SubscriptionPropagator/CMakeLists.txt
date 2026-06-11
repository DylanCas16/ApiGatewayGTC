cmake_minimum_required(VERSION 3.17)
 
add_library(SubscriptionPropagator STATIC
    monitor_propagator.cpp
    alarm_propagator.cpp
    log_propagator.cpp

)
 
target_include_directories(SubscriptionPropagator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${IDL_GEN_DIR}
    ${CMAKE_BINARY_DIR}/server/generated
    ${ACE_ROOT}/include
    ${TAO_ROOT}/include
    ${TAO_ROOT}/orbsvcs
)
 
target_link_libraries(SubscriptionPropagator PUBLIC
    SubscriptionRegistry
    lib_idl_stubs
    gRPC::grpc++
    protobuf::libprotobuf
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypes
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypesCorba
    TAO
    ACE
)
 
target_link_directories(SubscriptionPropagator PUBLIC
    ${ACE_ROOT}/lib
    ${TAO_ROOT}/lib
)
