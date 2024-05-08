cmake_minimum_required(VERSION 3.20)
project(mcc VERSION 0.0.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# MCC Lib
file(GLOB_RECURSE MCC_LIB_SOURCES "lib/*.c")

add_library(mcc_lib SHARED ${MCC_LIB_SOURCES})
target_include_directories(mcc_lib PUBLIC include)

if (MSVC)
    set(MCC_FLAGS "${CMAKE_C_FLAGS} /W4")
else ()
    set(MCC_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
endif ()
set_target_properties(mcc_lib PROPERTIES COMPILE_FLAGS ${MCC_FLAGS})

# MCC APP
# add_executable(mcc "mcc/mcc.c")
# target_link_libraries(mcc PRIVATE mcc_lib)

# MCC Tests
enable_testing()
add_subdirectory("test")
