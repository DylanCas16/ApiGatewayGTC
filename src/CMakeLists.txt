cmake_minimum_version(VERSION 3.16)
project(gcs_api_gateway CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})


find_package(MiddlewarePkg REQUIRED)
find_package(ACE  REQUIRED)
find_package(TAO  REQUIRED)
find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)

add_subdirectory(corba)
add_subdirectory(adapter)
add_subdirectory(server)