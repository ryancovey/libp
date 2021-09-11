# Compile definitions for compiled boost
# libraries were transcribed from their
# build/Jamefile.v2 file.

set(boost_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}/boost/libs")

add_library(boost_include INTERFACE)
target_include_directories(boost_include
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/boost"
)
target_compile_definitions(boost_include
    # Issue similar to https://github.com/UCL/STIR/issues/209
    INTERFACE BOOST_MATH_DISABLE_FLOAT128
)
target_compile_features(boost_include INTERFACE cxx_std_14)

set(boost_chrono_src "${boost_libs_dir}/chrono/src" )
add_library(boost_chrono STATIC
    "${boost_chrono_src}/chrono.cpp"
    "${boost_chrono_src}/process_cpu_clocks.cpp"
    "${boost_chrono_src}/thread_clock.cpp"
)
target_link_libraries(boost_chrono
    PUBLIC boost_include
    PUBLIC boost_system
)
target_compile_definitions(boost_chrono
    PUBLIC BOOST_CHRONO_STATIC_LINK=1
)

# The static lib boost system is resulting in linking
# errors. Using it as a header only library instead.
#set( boost_system_src "${boost_libs_dir}/system/src" )
#add_library( boost_system STATIC
#    "${boost_system_src}/error_code.cpp"
#)
#target_link_libraries( boost_system
#    PUBLIC boost_include
#)
#target_compile_definitions( boost_system
#    PUBLIC BOOST_SYSTEM_STATIC_LINK=1
#)
#set_target_properties( boost_system PROPERTIES
#    CXX_STANDARD 11
#    CXX_STANDARD_REQUIRED ON
#)

add_library(boost_system INTERFACE)
target_compile_definitions(boost_system
    INTERFACE BOOST_ERROR_CODE_HEADER_ONLY
)
target_link_libraries(boost_system
    INTERFACE boost_include
)

set(boost_timer_src "${boost_libs_dir}/timer/src")
add_library(boost_timer STATIC
    "${boost_timer_src}/auto_timers_construction.cpp"
    "${boost_timer_src}/cpu_timer.cpp"
)
target_link_libraries(boost_timer
    PUBLIC boost_include
    PRIVATE boost_chrono
    PRIVATE boost_system
)
target_compile_definitions(boost_timer
    PUBLIC BOOST_TIMER_STATIC_LINK=1
)

set(boost_test_src "${boost_libs_dir}/test/src")
add_library(boost_test STATIC
    "${boost_test_src}/compiler_log_formatter.cpp"
    "${boost_test_src}/debug.cpp"
    "${boost_test_src}/decorator.cpp"
    "${boost_test_src}/execution_monitor.cpp"
    "${boost_test_src}/framework.cpp"
    "${boost_test_src}/plain_report_formatter.cpp"
    "${boost_test_src}/progress_monitor.cpp"
    "${boost_test_src}/results_collector.cpp"
    "${boost_test_src}/results_reporter.cpp"
    "${boost_test_src}/test_framework_init_observer.cpp"
    "${boost_test_src}/test_tools.cpp"
    "${boost_test_src}/test_tree.cpp"
    "${boost_test_src}/unit_test_log.cpp"
    "${boost_test_src}/unit_test_main.cpp"
    "${boost_test_src}/unit_test_monitor.cpp"
    "${boost_test_src}/unit_test_parameters.cpp"
    "${boost_test_src}/junit_log_formatter.cpp"
    "${boost_test_src}/xml_log_formatter.cpp"
    "${boost_test_src}/xml_report_formatter.cpp"
)
target_link_libraries(boost_test
    PUBLIC boost_include
    PRIVATE boost_timer
)
target_compile_definitions(boost_test
    INTERFACE BOOST_TEST_NO_AUTO_LINK=1
    PUBLIC BOOST_TEST_STATIC_LINK=1
)

