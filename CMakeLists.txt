cmake_minimum_required(VERSION 3.15)
project(r_type)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML COMPONENTS graphics audio window system REQUIRED)
find_package(Boost REQUIRED)

add_executable(r_type src/main.cpp src/Systems.cpp)
add_executable(server src/server/main.cpp src/server/server.cpp)
add_executable(client src/client/main.cpp src/client/client.cpp)
target_link_libraries(server sfml-graphics sfml-window sfml-system)
