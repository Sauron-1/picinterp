cmake_minimum_required(VERSION 3.11)
project(picinterp)

option(BUILD_PYTHON "Whether to build python library" OFF)
option(INSTALL_CPP "Whether install C++ header file" ON)

set(MARCH "core-avx2" CACHE STRING "Architecture to tell gcc to optimize for (-march)")

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

add_library(picinterp INTERFACE)
target_include_directories(
    picinterp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/picinterp>
    $<INSTALL_INTERFACE:include/picinterp>
)

# require tuple_arithmetic library. Download from github if not found.
find_package(tuple_arithmetic QUIET)
if(NOT tuple_arithmetic_FOUND)
    message(STATUS "tuple_arithmetic not found. Downloading from github...")
    include(FetchContent)
    FetchContent_Declare(
        tuple_arithmetic
        GIT_REPOSITORY https://github.com/Sauron-1/tuple_arithmetic.git
        GIT_TAG        master
    )
    set(BULID_TESTS_TMP ${BUILD_TESTS})
    set(BUILD_TESTS OFF)
    FetchContent_MakeAvailable(tuple_arithmetic)
    set(BUILD_TESTS ${BULID_TESTS_TMP})
endif()
target_link_libraries(picinterp INTERFACE tuple_arithmetic)

if (INSTALL_CPP)
    install(TARGETS picinterp EXPORT picinterpConfig)
    install(EXPORT picinterpConfig DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/picinterp)
    install(DIRECTORY include DESTINATION include/picinterp)
    export(TARGETS picinterp FILE picinterpConfig.cmake)
endif()

if(BUILD_PYTHON)
    add_subdirectory(src)
endif()
