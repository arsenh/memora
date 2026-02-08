cmake_minimum_required(VERSION 3.16)

project(memora_server LANGUAGES CXX)

find_package(Boost REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/server/server.cpp
    src/server/session.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(${PROJECT_NAME}
    PUBLIC cxx_std_23
)

target_link_libraries(${PROJECT_NAME} PRIVATE boost::boost pthread)
