add_library(lib_any_adapter STATIC 
    any_adapter.cpp
    monitor_adapter.cpp
)

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
    lib_idl_stubs
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypes
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypesCorba
    TAO_AnyTypeCode
    TAO
    ACE
)