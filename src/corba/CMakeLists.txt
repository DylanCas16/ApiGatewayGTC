add_subdirectory(NamingService)
add_subdirectory(InterfaceRepository)
add_subdirectory(DII)
add_subdirectory(Config)

add_library(lib_gcs_endpoints STATIC
    gcs_endpoints.cpp
)

target_include_directories(lib_gcs_endpoints PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_directories(lib_gcs_endpoints PUBLIC
    ${ACE_ROOT}/lib
    ${TAO_ROOT}/lib
)

add_library(lib_corba_runtime STATIC
    corba_runtime.cpp
)

target_include_directories(lib_corba_runtime PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $ENV{ACE_ROOT}
    $ENV{TAO_ROOT}
    $ENV{TAO_ROOT}/orbsvcs
)
 
target_link_libraries(lib_corba_runtime PUBLIC
    lib_ns_resolver
    lib_ifr_client
    lib_dii_invoke
    lib_config_service
    TAO_PortableServer
    ACE
)
