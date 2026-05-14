cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_DISABLE_PRECOMPILED_HEADERS ON)

project(IntegratedDemonlist VERSION 1.7.13)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage("gh:hiimjasmine00/jasmine-tools@1.5.0")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/classes/IDPackLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/classes/IDPackCell.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} jasmine-tools)

# Fix for pugi::xml_node destructor issue
# Suppress the dtor-name warning for implicitly declared destructors in pugixml bindings
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wno-dtor-name
    -Wno-error=dtor-name
)

setup_geode_mod(${PROJECT_NAME})
