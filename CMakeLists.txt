cmake_minimum_required(VERSION 3.15)
project(r_type)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML COMPONENTS graphics audio window system REQUIRED)
find_package(Boost REQUIRED)

file(GLOB SERVER_SOURCES "src/server/*.cpp")
add_executable(r-type_server ${SERVER_SOURCES} src/Systems.cpp src/server/Mediator.cpp)
file(GLOB CLIENT_SOURCES "src/client/*.cpp")
add_executable(r-type_client ${CLIENT_SOURCES} src/Systems.cpp src/server/Mediator.cpp)
target_link_libraries(r-type_server sfml-graphics boost::boost)
target_link_libraries(r-type_client sfml-graphics sfml-window sfml-audio sfml-system boost::boost)