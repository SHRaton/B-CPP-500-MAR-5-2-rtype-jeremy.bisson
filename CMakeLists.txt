cmake_minimum_required(VERSION 3.15)
project(r_type)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML COMPONENTS graphics audio window system REQUIRED)
find_package(Boost REQUIRED)

add_executable(server src/server/main.cpp src/server/server.cpp)
file(GLOB CLIENT_SOURCES "src/client/*.cpp")
add_executable(client ${CLIENT_SOURCES} src/Systems.cpp)
target_link_libraries(server boost::boost)
target_link_libraries(client sfml-graphics sfml-window sfml-system boost::boost)