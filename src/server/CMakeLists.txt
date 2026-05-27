set(PROTO_BINARY_DIR  "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(PROTO_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/proto")

file(MAKE_DIRECTORY "${PROTO_BINARY_DIR}/proto")

add_library(proto-objects OBJECT
    "${CMAKE_CURRENT_LIST_DIR}/proto/server.proto"
    "${CMAKE_CURRENT_LIST_DIR}/proto/adapter.proto"
    "${CMAKE_CURRENT_LIST_DIR}/proto/unary.proto"
    "${CMAKE_CURRENT_LIST_DIR}/proto/config.proto"
    "${CMAKE_CURRENT_LIST_DIR}/proto/stream.proto"
)

target_link_libraries(proto-objects PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
)

target_include_directories(proto-objects PUBLIC
    "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>"
)

protobuf_generate(
    TARGET proto-objects
    IMPORT_DIRS ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
    APPEND_PATH FALSE
)

find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)
find_program(PROTOC_BIN protoc REQUIRED)

set(GRPC_GENERATED_FILES "")
foreach(PROTO_FILE server.proto adapter.proto unary.proto config.proto stream.proto)
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    set(GRPC_H "${PROTO_BINARY_DIR}/${PROTO_NAME}.grpc.pb.h")
    set(GRPC_CC "${PROTO_BINARY_DIR}/${PROTO_NAME}.grpc.pb.cc")

    add_custom_command(
        OUTPUT ${GRPC_H} ${GRPC_CC}
        COMMAND ${PROTOC_BIN}
            --grpc_out=${PROTO_BINARY_DIR}
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
            -I${PROTO_IMPORT_DIRS}
            -I${PROTO_BINARY_DIR}
            ${PROTO_IMPORT_DIRS}/${PROTO_FILE}
        DEPENDS ${PROTO_IMPORT_DIRS}/${PROTO_FILE}
        COMMENT "Generating gRPC stubs for ${PROTO_FILE}"
    )

    list(APPEND GRPC_GENERATED_FILES ${GRPC_H} ${GRPC_CC})
endforeach()

add_custom_target(grpc_gen DEPENDS ${GRPC_GENERATED_FILES})
add_dependencies(proto-objects grpc_gen)

target_sources(proto-objects PRIVATE ${GRPC_GENERATED_FILES})

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
    protobuf::libprotobuf
    gRPC::grpc++
)