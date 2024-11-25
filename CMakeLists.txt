cmake_minimum_required(VERSION 3.24)
project(r_type)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.6 COMPONENTS graphics audio window system REQUIRED)
find_package(Boost REQUIRED)


set(SOURCE_FILES src/main.cpp
        src/Systems.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# Répertoire où SFML place ses fichiers DLL
if(WIN32)
    set(SFML_DLL_DIR "${SFML_BINARY_DIR}/lib/Release")

    # Copier les fichiers DLL dans le dossier de l'exécutable
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SFML_DLL_DIR}/sfml-graphics-2.dll
            ${SFML_DLL_DIR}/sfml-window-2.dll
            ${SFML_DLL_DIR}/sfml-system-2.dll
            ${SFML_DLL_DIR}/sfml-network-2.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            )
endif()