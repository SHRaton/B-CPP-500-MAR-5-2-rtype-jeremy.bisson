file(GLOB CLIENT_SOURCES "*.cpp")
add_executable(r-type_client ${CLIENT_SOURCES} ../../src/Systems.cpp ../../src/server/Mediator.cpp)
target_link_libraries(r-type_client sfml-graphics sfml-window sfml-audio sfml-system boost::boost)