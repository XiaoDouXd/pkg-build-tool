cmake_minimum_required(VERSION 3.24.0)
project(rc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
file(GLOB_RECURSE GEN_RES_FILES "./res/*.cpp")

include_directories(${PROJECT_NAME} ./res)
add_library(${PROJECT_NAME} STATIC ${GEN_RES_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "./res")
