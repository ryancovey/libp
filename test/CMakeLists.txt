cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Only compile tests if dependencies have been downloaded.

if (TARGET boost_unit_test_framework)
    add_executable(tests
        "interval_test.cpp"
        "test_main.cpp"
    )
    target_link_libraries(tests
    	PUBLIC boost_include
        PUBLIC boost_unit_test_framework
        PUBLIC libp
    )
endif()

