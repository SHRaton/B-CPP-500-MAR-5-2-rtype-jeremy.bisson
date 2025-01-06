cmake_minimum_required(VERSION 3.15)
project(r_type)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML COMPONENTS graphics audio window system REQUIRED)
find_package(Boost REQUIRED)
find_package(Lua REQUIRED)
find_package(sol2 REQUIRED)
find_package(nlohmann_json REQUIRED)

add_subdirectory(src/client)
add_subdirectory(src/server)
