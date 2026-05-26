set(PROTO_BINARY_DIR  "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(PROTO_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/proto")
set(GRPC_CPP_PLUGIN_EXECUTABLE "/usr/bin/grpc_cpp_plugin")

add_library(proto-objects OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/proto/server.proto"
    "${CMAKE_CURRENT_LIST_DIR}/proto/adapter.proto"
)

target_link_libraries(proto-objects PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
)

target_include_directories(proto-objects PUBLIC
    "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>"
)

protobuf_generate(
    TARGET         proto-objects
    IMPORT_DIRS    ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)

protobuf_generate(
    TARGET proto-objects
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "${GRPC_CPP_PLUGIN_EXECUTABLE}"
    IMPORT_DIRS ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)

add_executable(gateway_server
    main.cpp
    unary_server.cpp
)

target_include_directories(gateway_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/corba
    ${CMAKE_SOURCE_DIR}/adapter
    ${PROTO_BINARY_DIR}
)

target_link_libraries(gateway_server PRIVATE
    lib_corba_runtime
    lib_any_adapter
    proto-objects
    gRPC::grpc++
    protobuf::libprotobuf
)