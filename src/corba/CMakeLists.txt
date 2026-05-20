cmake_minimum_required(VERSION 3.16)
project(gcs_corba CXX)
 
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
 
set(ACE_ROOT $ENV{ACE_ROOT})
set(TAO_ROOT $ENV{TAO_ROOT})
set(ACE_LIB_DIR $ENV{ACE_ROOT}/lib)

add_subdirectory(NamingService)
add_subdirectory(InterfaceRepository)
add_subdirectory(DII)

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
    TAO_PortableServer
    ACE
)

option(GATEWAY_TEST "Tests for CORBA" OFF)
if(GATEWAY_TEST)
    add_subdirectory(test)
endif()