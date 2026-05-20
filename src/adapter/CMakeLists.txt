add_library(lib_any_adapter STATIC
    any_adapter.cpp
)
 
target_include_directories(lib_any_adapter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
)
 
target_link_directories(lib_any_adapter PUBLIC
    $ENV{ACE_ROOT}/lib
)
 
target_link_libraries(lib_any_adapter PUBLIC
    lib_proto
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypes
    MiddlewarePkg::IL-DistributedKit-DGCSTypes-l-DGCSTypesCorba
    TAO_AnyTypeCode
    TAO
    ACE
)