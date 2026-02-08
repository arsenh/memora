cmake_minimum_required(VERSION 3.16)

project(memora_server LANGUAGES CXX)

find_package(Boost REQUIRED)

add_executable(memora_server
    src/main.cpp
    src/server/server.cpp
)

target_include_directories(memora_server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(memora_server
    PUBLIC cxx_std_23
)

target_link_libraries(memora_server PRIVATE boost::boost pthread)
