cmake_minimum_required(VERSION 3.20)
project(mcc VERSION 0.0.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_executable(mcc ${C_SOURCES})

if (MSVC)
    set_target_properties(mcc PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else ()
    set_target_properties(mcc PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif ()

target_include_directories(mcc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
