cmake_minimum_required(VERSION 3.10)
project(dii_invocation CXX)

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

add_executable(dii_invoke dii_invocation.cpp)

target_link_libraries(dii_invoke
    TAO_DynamicInterface
    TAO_Messaging
    TAO_Valuetype
    TAO_IFR_Client
    TAO_CosNaming
    TAO_AnyTypeCode
    TAO
    ACE
)
