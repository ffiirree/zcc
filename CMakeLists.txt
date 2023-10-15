cmake_minimum_required(VERSION 3.16)

project(zcc VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build type: Release." FORCE)
endif()

configure_file(src/version.h.in ${PROJECT_BINARY_DIR}/version.h @ONLY)

aux_source_directory(src SRC_LISTS)

add_executable(
    ${PROJECT_NAME}
    ${SRC_LISTS}
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${PROJECT_BINARY_DIR}
)

target_compile_options(${PROJECT_NAME} 
    PRIVATE 
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8 /DUNICODE /D_UNICODE /DNOMINMAX /Zc:preprocessor /Zc:__cplusplus /wd5054>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wno-deprecated-enum-enum-conversion>
)

