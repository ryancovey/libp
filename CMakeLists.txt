cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# See "Package Manager" section of Effective CMake
# conference talk.
# Video:
# https://www.youtube.com/watch?v=bsXLMQ6WgIk
# Slides:
# https://github.com/boostcon/cppnow_presentations_2017/blob/master/05-19-2017_friday/effective_cmake__daniel_pfeifer__cppnow_05-19-2017.pdf

project(libp)
macro(project)
    if (NOT "${ARGV0}" STREQUAL "libp")
        _project(${ARGV})
    endif()
endmacro()

add_library(libp INTERFACE)
target_include_directories(libp INTERFACE include)
target_compile_features(libp INTERFACE cxx_std_14)

add_subdirectory(external)
add_subdirectory(examples)
add_subdirectory(test)

