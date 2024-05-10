set(TESTS
    String
    Vector
    Scan
)

foreach(TEST IN LISTS TESTS)
    add_executable(${TEST} "${TEST}.c")
    target_link_libraries(${TEST} PRIVATE mcc_lib)
    target_include_directories(${TEST} PRIVATE .)

    add_test(NAME ${TEST} COMMAND ${TEST})
    add_custom_command(
        TARGET ${TEST}
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_CTEST_COMMAND} -R "^${TEST}\$" --output-on-failure
    )
endforeach()
