set(ACE_ROOT $ENV{ACE_ROOT})
set(TAO_ROOT $ENV{TAO_ROOT})

set(MIDDLEWARE_IDL_DIR
    "/home/gcsdev/.conan/data/MiddlewarePkg/1.0.0/gcs/stable/package/46bdbc50915a2cb97ed33e9f4dc66df67296e6d5/include"
)
set(IDL_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/idl")
file(MAKE_DIRECTORY ${IDL_GEN_DIR})

set(IDL_FILES
    ${MIDDLEWARE_IDL_DIR}/DGT.idl
    ${MIDDLEWARE_IDL_DIR}/CONFIG.idl
    ${MIDDLEWARE_IDL_DIR}/CONFIGManager.idl
    ${MIDDLEWARE_IDL_DIR}/ALARM.idl
    ${MIDDLEWARE_IDL_DIR}/ALARMManager.idl
    ${MIDDLEWARE_IDL_DIR}/LOG.idl
    ${MIDDLEWARE_IDL_DIR}/LOGManager.idl
    ${MIDDLEWARE_IDL_DIR}/MMCommon.idl
    ${MIDDLEWARE_IDL_DIR}/MonitorManager.idl
    ${MIDDLEWARE_IDL_DIR}/DCF.idl
)

set(IDL_GENERATED_HEADERS "")

foreach(IDL_FILE ${IDL_FILES})
    get_filename_component(IDL_NAME ${IDL_FILE} NAME_WE)

    add_custom_command(
        OUTPUT
            "${IDL_GEN_DIR}/${IDL_NAME}C.h"
            "${IDL_GEN_DIR}/${IDL_NAME}S.h"
        COMMAND tao_idl
            -I$ENV{TAO_ROOT}/TAO_IDL
            -I${MIDDLEWARE_IDL_DIR}
            -o ${IDL_GEN_DIR}
            ${IDL_FILE}
        DEPENDS ${IDL_FILE}
        COMMENT "Generating IDL stubs for ${IDL_NAME}"
    )

    list(APPEND IDL_GENERATED_HEADERS
        "${IDL_GEN_DIR}/${IDL_NAME}C.h"
        "${IDL_GEN_DIR}/${IDL_NAME}S.h"
    )
endforeach()

add_custom_target(generate_idl_stubs DEPENDS ${IDL_GENERATED_HEADERS})

add_library(lib_any_adapter STATIC any_adapter.cpp)

add_dependencies(lib_any_adapter generate_idl_stubs proto-objects)

target_include_directories(lib_any_adapter PUBLIC
    ${ACE_ROOT}/include
    ${TAO_ROOT}/include
    ${TAO_ROOT}/orbsvcs
    ${CMAKE_BINARY_DIR}/server/generated
    ${IDL_GEN_DIR}
)

target_link_directories(lib_any_adapter PUBLIC
    ${ACE_ROOT}/lib
    ${TAO_ROOT}/lib
)

target_link_libraries(lib_any_adapter PUBLIC
    proto-objects
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypes
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypesCorba
    TAO_AnyTypeCode
    TAO
    ACE
)