cmake_minimum_required(VERSION 3.10)
project(ifr_connect CXX)

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

add_executable(ifr_data ifr_connect.cpp)

target_link_libraries(ifr_data
    TAO_IFR_Client
    TAO_CosNaming
    TAO_AnyTypeCode
    TAO
    ACE
)
