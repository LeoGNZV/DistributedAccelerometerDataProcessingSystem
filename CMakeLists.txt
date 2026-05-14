cmake_minimum_required(VERSION 3.17)
project(AccelerometerSystem)
set(CMAKE_CXX_STANDARD 17)
find_package(nlohmann_json REQUIRED)

add_library(common
common/src/json_utils.cpp
common/src/logger.cpp
common/src/tcp_socket.cpp
)

target_include_directories(common PUBLIC common/include)
target_link_libraries(common PUBLIC nlohmann_json::nlohmann_json)

add_subdirectory(server)
add_subdirectory(node_a)
add_subdirectory(node_b)
