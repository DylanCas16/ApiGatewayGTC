set(ACE_ROOT $ENV{ACE_ROOT})
set(TAO_ROOT $ENV{TAO_ROOT})

add_library(lib_any_adapter STATIC
    any_adapter.cpp
)

add_dependencies(lib_any_adapter proto-objects)

target_include_directories(lib_any_adapter PUBLIC
    ${ACE_ROOT}/include
    ${TAO_ROOT}/include
    ${TAO_ROOT}/orbsvcs
    ${CMAKE_BINARY_DIR}/server/generated
)

target_link_directories(lib_any_adapter PUBLIC
    ${ACE_ROOT}/lib
    ${TAO_ROOT}/lib
)

target_link_libraries(lib_any_adapter PUBLIC
    lib_proto
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypes
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypesCorba
    TAO_AnyTypeCode
    TAO
    ACE
)