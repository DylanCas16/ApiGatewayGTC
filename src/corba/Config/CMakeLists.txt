add_library(lib_config_service STATIC
    config_service.cpp
)

add_dependencies(lib_config_service generate_idl_stubs)

target_include_directories(lib_config_service PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
    $ENV{TAO_ROOT}/orbsvcs
    ${IDL_GEN_DIR}
)

target_link_directories(lib_config_service PUBLIC
    $ENV{ACE_ROOT}/lib
)

target_link_libraries(lib_config_service PUBLIC
    lib_idl_stubs
    TAO_CosNaming
    TAO_AnyTypeCode
    TAO
    ACE
)
