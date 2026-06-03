cmake_minimum_required(VERSION 3.17)
project(gcs_api_gateway CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
find_package(MiddlewarePkg REQUIRED)
find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)
find_program(TAO_IDL tao_idl PATHS $ENV{ACE_ROOT}/bin REQUIRED)

set(ACE_ROOT $ENV{ACE_ROOT})
set(TAO_ROOT $ENV{TAO_ROOT})

set(MIDDLEWARE_IDL_DIR
    "/home/gcsdev/.conan/data/MiddlewarePkg/1.0.0/gcs/stable/package/46bdbc50915a2cb97ed33e9f4dc66df67296e6d5/include"
    CACHE PATH "GCS middleware IDL directory"
)
set(IDL_GEN_DIR "${CMAKE_BINARY_DIR}/generated/idl"
    CACHE PATH "IDL generated output"
)
file(MAKE_DIRECTORY ${IDL_GEN_DIR})

set(IDL_FILES
    ${MIDDLEWARE_IDL_DIR}/DGT.idl
    ${MIDDLEWARE_IDL_DIR}/SK.idl
    ${MIDDLEWARE_IDL_DIR}/CONFIG.idl
    ${MIDDLEWARE_IDL_DIR}/ALARM.idl
    ${MIDDLEWARE_IDL_DIR}/LOG.idl
    ${MIDDLEWARE_IDL_DIR}/MMCommon.idl
    ${MIDDLEWARE_IDL_DIR}/CONFIGManager.idl
    ${MIDDLEWARE_IDL_DIR}/ALARMManager.idl
    ${MIDDLEWARE_IDL_DIR}/LOGManager.idl
    ${MIDDLEWARE_IDL_DIR}/MonitorManager.idl
    ${MIDDLEWARE_IDL_DIR}/DCF.idl
)

set(IDL_STUB_SOURCES "")
set(IDL_GENERATED_HEADERS "")

foreach(IDL_FILE ${IDL_FILES})
    get_filename_component(IDL_NAME ${IDL_FILE} NAME_WE)

    add_custom_command(
        OUTPUT
            "${IDL_GEN_DIR}/${IDL_NAME}C.h"
            "${IDL_GEN_DIR}/${IDL_NAME}C.cpp"
            "${IDL_GEN_DIR}/${IDL_NAME}C.inl"
            "${IDL_GEN_DIR}/${IDL_NAME}S.h"
            "${IDL_GEN_DIR}/${IDL_NAME}S.cpp"
        COMMAND ${TAO_IDL}
                -I${MIDDLEWARE_IDL_DIR}
                -I$ENV{TAO_ROOT}/orbsvcs
                -o ${IDL_GEN_DIR}
                ${IDL_FILE}
        DEPENDS ${IDL_FILE}
        COMMENT "Generating IDL stubs for ${IDL_NAME}"
    )

    list(APPEND IDL_STUB_SOURCES      "${IDL_GEN_DIR}/${IDL_NAME}C.cpp")
    list(APPEND IDL_GENERATED_HEADERS "${IDL_GEN_DIR}/${IDL_NAME}C.h")
endforeach()

add_custom_target(generate_idl_stubs DEPENDS ${IDL_GENERATED_HEADERS})

add_library(lib_idl_stubs STATIC ${IDL_STUB_SOURCES})
add_dependencies(lib_idl_stubs generate_idl_stubs)

target_include_directories(lib_idl_stubs PUBLIC
    "${IDL_GEN_DIR}"
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
    $ENV{TAO_ROOT}/orbsvcs
)
target_link_libraries(lib_idl_stubs PUBLIC TAO ACE)

set(IDL_SKELETON_SOURCES "")
foreach(IDL_FILE ${IDL_FILES})
    get_filename_component(IDL_NAME ${IDL_FILE} NAME_WE)
    list(APPEND IDL_SKELETON_SOURCES "${IDL_GEN_DIR}/${IDL_NAME}S.cpp")
endforeach()
 
add_library(lib_idl_skeletons STATIC ${IDL_SKELETON_SOURCES})
add_dependencies(lib_idl_skeletons generate_idl_stubs)
target_include_directories(lib_idl_skeletons PUBLIC
    "${IDL_GEN_DIR}"
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
    $ENV{TAO_ROOT}/orbsvcs
)
target_link_libraries(lib_idl_skeletons PUBLIC lib_idl_stubs TAO_PortableServer)

add_subdirectory(corba)
add_subdirectory(adapter)
add_subdirectory(server)
add_subdirectory(CorbaServant)
add_subdirectory(SubscriptionRegistry)
add_subdirectory(SubscriptionPropagator)