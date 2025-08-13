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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/picinterp>
)

find_package(tuple_arithmetic QUIET)
if (NOT tuple_arithmetic_FOUND)
    message("tuple_arithmetic not found. Using built-in")
    add_subdirectory(thirdparty/tuple_arithmetic)
else()
    message("tuple_arithmetic found.")
endif()
target_link_libraries(picinterp INTERFACE tuple_arithmetic)

if (INSTALL_CPP)
    install(TARGETS picinterp EXPORT picinterpConfig)
    install(EXPORT picinterpConfig DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/picinterp)
    install(DIRECTORY include/ DESTINATION include/picinterp)
    export(TARGETS picinterp FILE picinterpConfig.cmake)
endif()

if(BUILD_PYTHON)
    add_subdirectory(src)
endif()
