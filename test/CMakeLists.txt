cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Only compile tests if dependencies have been downloaded.

if (TARGET boost_test)
    add_executable(tests
        "sets.cpp"
        "test_main.cpp"
    )
    target_link_libraries(tests
        PUBLIC boost_test
        PUBLIC libp
    )
endif()

