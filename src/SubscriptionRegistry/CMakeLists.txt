cmake_minimum_required(VERSION 3.17)
 
add_library(SubscriptionRegistry INTERFACE)
 
target_include_directories(SubscriptionRegistry INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
 
target_link_libraries(SubscriptionRegistry INTERFACE
    gRPC::grpc++
)