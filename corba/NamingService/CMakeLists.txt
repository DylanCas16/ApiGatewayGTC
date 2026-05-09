add_library(lib_ns_resolver STATIC
    ns_discover.cpp
)
 
target_include_directories(lib_ns_resolver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
    $ENV{TAO_ROOT}/orbsvcs
)
 
link_directories($ENV{ACE_ROOT}/lib)
 
target_link_libraries(lib_ns_resolver PUBLIC
    TAO_CosNaming
    TAO_AnyTypeCode
    TAO
    ACE
)
