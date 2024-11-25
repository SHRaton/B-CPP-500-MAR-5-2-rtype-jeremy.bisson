cmake_minimum_required(VERSION 3.24)
project(r_type)

set(CMAKE_CXX_STANDARD 17)


if(WIN32)
    include(FetchContent)

    FetchContent_Declare(
            SFML
            URL https://www.sfml-dev.org/files/SFML-2.6.2-sources.zip
    )

    FetchContent_MakeAvailable(SFML)
endif()

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