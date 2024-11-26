# Projet r_type

## Description
Le projet **r_type** est un jeu ou une application développée en C++, utilisant SFML pour le rendu graphique et la gestion des événements. Ce projet utilise CMake pour la gestion de la compilation et des dépendances.

## Prérequis
Avant de commencer, assurez-vous que vous avez installé les outils suivants :
- **CMake** (version 3.24 ou plus récente)
- **Un compilateur C++** (par exemple, g++ ou clang)

## Étapes pour lancer le projet

### 1. Cloner le projet
Clonez ce projet dans un répertoire local :

```bash
git clone https://github.com/EpitechPromo2027/B-CPP-500-MAR-5-2-rtype-jeremy.bisson.git
cd B-CPP-500-MAR-5-2-rtype-jeremy.bisson-main
```

### 2. Build, Compiler et Executer
Vous devez lancer les commandes suivantes afin de build avec CMake :

Pour n'importe quels OS :

```
    conan profile detect --force
    conan install . --output-folder=build --build=missing
    cd build
```

**(Windows)**
-
```bash
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
cmake --build . --config Release
D'abord lancer le serveur
./Release/server.exe
Puis lancer le Client/Jeu
./Release/r-type.exe
```

`cmake .` : Cette commande configure votre projet et génère les fichiers nécessaires pour la construction. C'est l'étape où CMake va analyser votre CMakeLists.txt, résoudre les dépendances, et configurer les paramètres de compilation. (**A faire seulement la 1ere fois que vous lancez le projet**)

`cmake --build . --config Release` : Cette commande est celle qui lance réellement la compilation de votre projet en utilisant les fichiers générés lors de la première étape. Elle compile les sources, les lie, et crée l'exécutable ou la bibliothèque spécifiée dans votre CMakeLists.txt.

**(Linux)**
-
```bash
    cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=Release
    make
    D'abord lancer le serveur
    ./server port (le port sur lequel on veut héberger)
    Puis lancer le Client/Jeau
    ./r-type
```
`cmake .` : Cette commande configure votre projet et génère les fichiers nécessaires pour la construction. C'est l'étape où CMake va analyser votre CMakeLists.txt, résoudre les dépendances, et configurer les paramètres de compilation.

(**A faire seulement la 1ere fois que vous lancez le projet**)

`make` : Cette commande est celle qui lance réellement la compilation de votre projet en utilisant les fichiers générés lors de la première étape. Elle compile les sources, les lie, et crée l'exécutable ou la bibliothèque spécifiée dans votre CMakeLists.txt.

### Liste des règles du CMake

- **_Build en mode Normal_**
```
cmake --build . --config Release
```
- **_Build en mode Debug_**
```
cmake --build . --config Debug
```
- **_Clean les fichiers de Cache et les fichiers temporaires_**
```
cmake --build . --target clean-all
```
- **_Clean tout les fichiers inutiles puis Build en mode Normal_**
```
cmake --build . --config Release --target re
```