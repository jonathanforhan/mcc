cmake_minimum_required (VERSION 3.26)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(MCC LANGUAGES C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(MCC_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}")

################################################################################
# MCC
################################################################################

file(GLOB_RECURSE LIB_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c")
add_library(mcc_lib STATIC ${LIB_SRCS})
target_include_directories(mcc_lib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
)
target_compile_definitions(
    mcc_lib PUBLIC
        $<$<CONFIG:Debug>:MCC_DEBUG>
        $<$<CONFIG:Release>:MCC_RELEASE>
)

if (MSVC)
    target_compile_options(
    	mcc_lib PUBLIC
    		/Zc:preprocessor
    		/W4
    )
else()
    target_compile_options(
    	mcc_lib PUBLIC
    		-Wall
    		-Wextra
    		-Wpedantic
    		-Wconversion
    )
endif()

################################################################################
# App
################################################################################

file(GLOB_RECURSE APP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/app/*.c")
add_executable(mcc ${APP_SRCS})
target_link_libraries(mcc PRIVATE mcc_lib)
target_include_directories(mcc PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/app/")

################################################################################
# TESTING
################################################################################

#enable_testing()
add_subdirectory(tests)

execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/compile_commands.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json"
)
