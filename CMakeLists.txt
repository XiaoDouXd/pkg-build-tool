cmake_minimum_required(VERSION 3.24.0)

project(smbx38a-starter-build-tool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(XD_RC_WINDRES_PATH icon)
if(WIN32 AND DEFINED XD_RC_WINDRES_PATH AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${XD_RC_WINDRES_PATH}")
    set(__RC_WINDRES_PATH_REAL_FULLPATH "${CMAKE_CURRENT_SOURCE_DIR}/${XD_RC_WINDRES_PATH}")
    file (GLOB_RECURSE ALL_RC "${XD_RC_WINDRES_PATH}/*.rc.in")
    foreach(rc_in ${ALL_RC})
        file(RELATIVE_PATH rc_in_name "${__RC_WINDRES_PATH_REAL_FULLPATH}" ${rc_in})
        string(REGEX REPLACE ".in$" "" rc_out ${rc_in_name})
        configure_file(${rc_in} "${CMAKE_CURRENT_SOURCE_DIR}/rc/gen_rc/${rc_out}")
    endforeach()
endif()

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

if(WIN32)
    file(GLOB_RECURSE ALL_RC "rc/gen_rc/*.rc")
    target_sources(${PROJECT_NAME} PRIVATE ${ALL_RC})
else()
    message(FATAL_ERROR  "${PROJECT_NAME}::CMakeLists::Error: Unsupported platform")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC rc)