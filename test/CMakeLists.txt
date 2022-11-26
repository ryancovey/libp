cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Only compile tests if dependencies have been downloaded.

if (TARGET boost_unit_test_framework)
    add_executable(tests
        "function_register_test.cpp"
        "interval_test.cpp"
        "sets_test.cpp"
        "test_main.cpp"
    )
    target_link_libraries(tests
    	PUBLIC boost_include
        PUBLIC boost_unit_test_framework
        PUBLIC libp
    )

    # Tests of the behaviour of static functions needs to be in its own
    # executable, since the behaviour we want to test is runs during
    # static initialisation if MeasurableSet objects exist in the
    # executable.
    add_executable(test_measurable_set_statics
        "test_measurable_set_statics.cpp"
    )
    target_link_libraries(test_measurable_set_statics
        PUBLIC boost_include
        PUBLIC boost_unit_test_framework
        PUBLIC libp
    )
endif()

