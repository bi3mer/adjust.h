cmake_minimum_required(VERSION 3.11)
project(bouncing-ball C)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build mode configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# Define build mode flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(MODE_PRODUCTION)
    message(STATUS "Building in Release mode - adjust compiled out")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(MODE_DEBUG)
    message(STATUS "Building in Debug mode - adjust hot-reloading enabled")
endif()

# Dependencies - Raylib
include(FetchContent)
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
    endif()
endif()

# Our Project
add_executable(${PROJECT_NAME} main.c)

# Include directories - point to the main adjust library directory
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..  # Path to main adjust directory
    # This resolves to /Users/colan/projects/libraries/adjust/
)

# Link libraries
target_link_libraries(${PROJECT_NAME} raylib)

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wstrict-prototypes
        -Wmissing-prototypes
    )
endif()

# macOS frameworks
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

# Custom target to run the program
add_custom_target(run
    COMMAND ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running ${PROJECT_NAME}..."
)

# Print configuration summary
message(STATUS "----------------------------------------")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS "----------------------------------------")
