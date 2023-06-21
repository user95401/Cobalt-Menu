cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ModMenuTest VERSION 1.0.0)

file(GLOB SOURCES
	src/*.cpp
)


add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

setup_geode_mod(${PROJECT_NAME})


CPMAddPackage("gh:matcool/gd-imgui-cocos#b0064c6") # specify a commit!

target_link_libraries(${PROJECT_NAME} imgui-cocos)

