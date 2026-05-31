add_library(lib_dii_invoke STATIC
    dii_invocation.cpp
    device_dispatcher.cpp
)

add_dependencies(lib_dii_invoke generate_idl_stubs)

target_include_directories(lib_dii_invoke PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
    $ENV{TAO_ROOT}/orbsvcs
    ${IDL_GEN_DIR}
)

target_link_directories(lib_dii_invoke PUBLIC
    $ENV{ACE_ROOT}/lib
)

target_link_libraries(lib_dii_invoke PUBLIC
    lib_idl_stubs
    TAO_DynamicInterface
    TAO_Messaging
    TAO_Valuetype
    TAO_IFR_Client
    TAO_AnyTypeCode
    TAO
    ACE
)
