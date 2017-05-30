
cmake_minimum_required(VERSION 3.1)
project(allocated_value CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS Off)

add_library(allocated_value INTERFACE)
target_include_directories(allocated_value INTERFACE
                           ${allocated_value_SOURCE_DIR}/include)

enable_testing()

add_executable(test_allocated_value
               test/test_allocated_value_basic.cpp
               test/test_allocated_value_nested.cpp
               test/test_allocated_value_odd_allocators.cpp
               test/test_allocated_value_odd_types.cpp
               test/test_allocated_value_pimpl.cpp
               test/test_allocated_value_stack_allocator.cpp
               test/test_pimpl.cpp
               test/catch_main.cpp)
target_link_libraries(test_allocated_value PUBLIC allocated_value)
add_test(test_allocated_value test_allocated_value)

add_executable(test_allocated_value_no_exceptions
               test/test_allocated_value_no_exceptions.cpp)
target_link_libraries(test_allocated_value_no_exceptions PUBLIC allocated_value)
target_compile_options(test_allocated_value_no_exceptions PUBLIC "-fno-exceptions")
add_test(test_allocated_value_no_exceptions test_allocated_value_no_exceptions)
