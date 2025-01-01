cmake_minimum_required(VERSION 3.30)
project(obs_camera_move LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

# OBS SDK path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
find_package(OBS REQUIRED)

# ==== ASIO ====
include(FetchContent)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(asio)

# Add source files
set (SOURCES
    src/library.cpp
    src/tcp_server.cpp
    src/tcp_connection.cpp
    src/logger.cpp
    src/message_handler.cpp
    src/message_command.cpp
    src/camera_controller.cpp
    src/string_utils.cpp
    src/env_var.cpp
)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Include obj library and includes
target_include_directories(${PROJECT_NAME} PRIVATE ${OBS_INCLUDE_DIR} ${asio_SOURCE_DIR}/asio/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${OBS_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OBS_LIBRARY})
