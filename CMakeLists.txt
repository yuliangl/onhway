cmake_minimum_required(VERSION 2.8)
project(ONHWAY)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -o3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unsed-function")

include_directories(onhway)
include_directories(/home/leon/apps/yaml-cpp/include)
#include_directories(user/include/pthread.h)

set(LIB_SRC 
    onhway/log.cc
    onhway/util.cc
    onhway/config.cc
    onhway/thread.cc
    )

add_library(onhway SHARED ${LIB_SRC})
#add_library(onhway_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES(onhway_static PROPERTIES OUTPUT_NAME "onhway")

add_executable(log tests/test.cc)
add_dependencies(log onhway)
target_link_libraries(log onhway yaml-cpp pthread)

add_executable(log_config tests/test_config.cc)
add_dependencies(log_config onhway)
target_link_libraries(log_config onhway yaml-cpp pthread)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
