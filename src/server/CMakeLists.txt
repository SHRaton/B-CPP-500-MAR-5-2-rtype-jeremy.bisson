file(GLOB SERVER_SOURCES "*.cpp")
add_executable(r-type_server ${SERVER_SOURCES} ../../src/Systems.cpp ../../src/server/Mediator.cpp)
target_link_libraries(r-type_server 
    sfml-graphics 
    boost::boost 
    lua::lua 
    sol2::sol2
    nlohmann_json::nlohmann_json)