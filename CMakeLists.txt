cmake_minimum_required(VERSION 3.24.0)

project(smbx38a-starter-build-tool)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(XD_RC_REGEN ON)
add_subdirectory(rc)

file(GLOB_RECURSE ALL_RES
        3rd/*.h
        3rd/*.hpp
        3rd/*.c
        3rd/*.cpp
        src/*.h
        src/*.hpp
        src/*.c
        src/*.cpp)
include_directories(./src ./src/private ./3rd/inireader)
add_executable(${PROJECT_NAME} ${ALL_RES})

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "smbx-38a-starter::CMakeLists::Info: msvc config success")
else()
    message(FATAL_ERROR  "${PROJECT_NAME}::CMakeLists::Error: Unsupported compiler")
endif()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

target_link_libraries(${PROJECT_NAME} PUBLIC rc)