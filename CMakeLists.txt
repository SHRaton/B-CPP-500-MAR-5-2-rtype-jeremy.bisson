cmake_minimum_required(VERSION 3.15)
project(r_type)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML COMPONENTS graphics audio window system REQUIRED)
find_package(Boost REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/Systems.cpp)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system boost::boost)