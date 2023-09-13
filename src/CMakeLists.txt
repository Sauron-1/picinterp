cmake_minimum_required(VERSION 3.11)
project(pypicinterp)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

find_package(pybind11 QUIET)
if (NOT pybind11_FOUND)
    message("-- pybind11 not found. Downloading...")
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        URL    https://github.com/pybind/pybind11/archive/refs/tags/v2.11.1.zip
        URL_HASH MD5=c62d9e05243bd31cdb3bae1bb2f56655
        TLS_VERIFY true
    )
    FetchContent_MakeAvailable(pybind11)
endif()
file(GLOB IMPL_SRC ${CMAKE_CURRENT_LIST_DIR}/impls/*.cpp)
file(GLOB SRC ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
pybind11_add_module(pypicinterp ${SRC} ${IMPL_SRC})

target_link_libraries(pypicinterp PRIVATE tuple_arithmetic)
target_include_directories(pypicinterp PRIVATE ${xsimd_INCLUDE_DIRS})

message("-- Compiling for march=${MARCH}")

target_include_directories(pypicinterp PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include)
target_compile_options(pypicinterp PRIVATE -O3 -march=${MARCH} -ffast-math -DNDEBUG)

find_package(OpenMP QUIET)
if(OpenMP_CXX_FOUND)
    message("-- OpenMP found, version ${OpenMP_CXX_VERSION}")
    target_link_libraries(pypicinterp PRIVATE OpenMP::OpenMP_CXX)
else()
    message("-- OpenMP not found. Performance maybe downgraded")
endif()

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

set(OUTDIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set_target_properties(pypicinterp PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTDIR})
set_target_properties(pypicinterp PROPERTIES OUTPUT_NAME picinterp)

install(TARGETS pypicinterp
    COMPONENT python
    LIBRARY DESTINATION lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
)
