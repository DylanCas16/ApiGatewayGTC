cmake_minimum_required(VERSION 3.10)
project(ns_list CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ACE_ROOT $ENV{ACE_ROOT})
set(TAO_ROOT $ENV{TAO_ROOT})

include_directories(
    ${ACE_ROOT}
    ${TAO_ROOT}
    ${TAO_ROOT}/orbsvcs
)

link_directories(
    ${ACE_ROOT}/lib
)

add_executable(ns_list ns_discover.cpp)

target_link_libraries(ns_list
    TAO_CosNaming
    TAO_AnyTypeCode
    TAO
    ACE
)
