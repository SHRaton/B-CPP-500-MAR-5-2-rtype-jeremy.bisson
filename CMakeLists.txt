# CMakeLists.txt principal
cmake_minimum_required(VERSION 3.10)
project(r-type VERSION 1.0)

# Spécifier le standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options de compilation
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Variables pour organiser les fichiers sources
set(SOURCES
    src/main.cpp
    #src/game.cpp
    #src/player.cpp
)

# set(HEADERS
#     include/game.hpp
#     include/player.hpp
# )

# Configuration de SFML
# Option pour choisir entre SFML statique ou dynamique
option(SFML_STATIC_LIBRARIES "Use static SFML libraries" OFF)

# Chercher SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Création de l'exécutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Inclusion des en-têtes
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

# Lier SFML
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        sfml-graphics 
        sfml-window 
        sfml-system
)

# Configuration spécifique à la plateforme
if(WIN32)
    # Configuration Windows

    # Copier les DLLs SFML dans le dossier de sortie (Windows uniquement)
    if(NOT SFML_STATIC_LIBRARIES)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:sfml-graphics>
                $<TARGET_FILE:sfml-window>
                $<TARGET_FILE:sfml-system>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endif()

elseif(UNIX AND NOT APPLE)
    # Configuration Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE X11)
endif()

# Configuration du répertoire des ressources
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/ressources")
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        RESOURCE_PATH="${RESOURCE_DIR}/"
)

# Copier les ressources dans le dossier build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/ressources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/ressources
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY ressources
    DESTINATION bin
)

# Packaging
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
include(CPack)