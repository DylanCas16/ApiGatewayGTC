add_library(lib_adapter STATIC 
    any_adapter.cpp
    monitor_adapter.cpp
    alarm_adapter.cpp
    log_adapter.cpp
)

add_dependencies(lib_adapter generate_idl_stubs proto-objects)

target_include_directories(lib_adapter PUBLIC
    ${ACE_ROOT}/include
    ${TAO_ROOT}/include
    ${TAO_ROOT}/orbsvcs
    ${CMAKE_BINARY_DIR}/server/generated
    ${IDL_GEN_DIR}
)

target_link_directories(lib_adapter PUBLIC
    ${ACE_ROOT}/lib
    ${TAO_ROOT}/lib
)

target_link_libraries(lib_adapter PUBLIC
    proto-objects
    lib_idl_stubs
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypes
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypesCorba
    TAO_AnyTypeCode
    TAO
    ACE
)