add_library(lib_dii_invoke STATIC
    dii_invocation.cpp
)

target_include_directories(lib_dii_invoke PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
    $ENV{TAO_ROOT}/orbsvcs
)

target_link_directories(lib_dii_invoke PUBLIC
    ${ACE_LIB_DIR}
)

target_link_libraries(lib_dii_invoke PUBLIC
    TAO_DynamicInterface
    TAO_Messaging
    TAO_Valuetype
    TAO_IFR_Client
    TAO_AnyTypeCode
    TAO
    ACE
)
