cmake_minimum_required(VERSION 3.16)
project(memora_server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(
        ${CMAKE_SOURCE_DIR}/src
)

# Source files
add_executable(${PROJECT_NAME}
        src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE pthread)

