add_library(lib_ifr_client STATIC
    ifr_connect.cpp
)
 
target_include_directories(lib_ifr_client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
    $ENV{TAO_ROOT}/orbsvcs
)
 
target_link_directories(lib_ifr_client PUBLIC
    ${ACE_LIB_DIR}
)

target_link_libraries(lib_ifr_client PUBLIC
    TAO_IFR_Client
    TAO_AnyTypeCode
    TAO
    ACE
)
