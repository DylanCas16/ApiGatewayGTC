add_executable(corba_test main.cpp)
 
target_include_directories(corba_test PRIVATE
    ${CMAKE_SOURCE_DIR}/corba
)
 
target_link_libraries(corba_test PRIVATE
    lib_corba_runtime
)
 