cmake_minimum_required(VERSION 3.24.0)

set(XD_RC_REGEN ON)

set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../build/)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR} CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_BUILD_FILES_DIRECTORY ${BUILD_DIR})
set(CMAKE_BUILD_DIRECTORY ${BUILD_DIR})
set(CMAKE_BINARY_DIR  ${BUILD_DIR})
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR})
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR}lib)
set(CMAKE_CACHEFILE_DIR ${BUILD_DIR})
set(CMAKE_BUILD_TYPE ${BUILD_TYPE})

project(exe-pkg)

# -------------------------------------------------------------- icon
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
# --------------------------------------------------------------------

add_subdirectory(rc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# ----------------------------------------------------------- add file
file(GLOB_RECURSE ALL_RES
        _3rd/zlib/*.h
        _3rd/zlib/*.c
        _3rd/minizip/*.h
        _3rd/minizip/*.c
        src/*.h
        src/*.hpp
        src/*.c
        src/*.cpp)
include_directories(./src ./src/private ./_3rd/zlib ./_3rd/minizip)
add_executable(${PROJECT_NAME} WIN32 ${ALL_RES})

if(WIN32)
    file(GLOB_RECURSE ALL_RC "rc/gen_rc/*.rc")
    target_sources(${PROJECT_NAME} PRIVATE ${ALL_RC})
else()
    message(FATAL_ERROR  "${PROJECT_NAME}::CMakeLists::Error: Unsupported platform")
endif()
# --------------------------------------------------------------------

# ------------------------------------------------------- linking flag
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_EXE_LINKER_FLAGS "/NODEFAULTLIB:\"LIBCMT.lib\"")
    message(STATUS "smbx-38a-starter::CMakeLists::Info: msvc config success")
else()
    message(FATAL_ERROR  "${PROJECT_NAME}::CMakeLists::Error: Unsupported compiler")
endif()
# --------------------------------------------------------------------

# ------------------------------------------------------------ add pkg
target_link_libraries(${PROJECT_NAME} PUBLIC rc)
# --------------------------------------------------------------------
