# Documentation Client R-Type

## Vue d'ensemble

Le Client est le programme du joueur qui se connecte au serveur, crée/rejoint une instance de jeu et affiche le jeu.

## Architecture

Le client est divisé en 3 parties principales :
- Affichage du jeu (Core et GUI)
- Gestion des connexions/communication avec le serveur (UDPNetworkClient)
- Système d'entités et composants (Registry)

## Affichage du jeu

### Structure des fichiers
```
src/client/
├── Core.cpp       # Gestion principale du jeu
└── GuiGame.cpp    # Interface graphique du jeu
```

### Fonctionnalités principales

La bibliothèque SFML est utilisée pour l'affichage du jeu. Le client dispose de plusieurs interfaces graphiques :
1. Menu principal
2. Écran de connexion 
3. Interface de jeu

#### Classe Core

La classe `Core` gère :
- Le chargement des ressources via `loadAssets()`
- L'initialisation de la fenêtre SFML
- La gestion des sprites et leur ordre d'affichage
- Le système de composants via un Registry

#### Collections de sprites

```cpp
sprites_menu     // Sprites du menu principal
sprites_login    // Sprites de l'écran de connexion  
sprites_game     // Sprites du jeu
```

#### Boucle principale

La fonction `gui_game()` gère :
- Les événements clavier
- La mise à jour des positions des joueurs
- L'affichage des sprites de fond
- L'affichage du vaisseau du joueur et des autres joueurs
- L'affichage des informations de FPS et latence

## Gestion des mouvements

La fonction `handleMoove()` implémente :

### Contrôles
- Touches directionnelles pour le déplacement
- Animation du sprite du vaisseau
- Envoi des nouvelles positions au serveur via UDP

### Exemple de code de mouvement
```cpp
if (keysPressed[sf::Keyboard::Up]) {
    movement.y -= baseSpeed * deltaSeconds;
    // Animation logic...
}
```

## Communication réseau 

### Classe UDPNetworkClient

Fonctionnalités :
- Connexion UDP avec le serveur
- Files d'attente de messages
- Thread dédié pour les opérations réseau

### Protocole

Messages principaux :
```
MOVE id x y     # Mise à jour de la position d'un joueur
CONNECT id      # Connexion d'un nouveau joueur
```

## Système d'entités (ECS)

### Components disponibles

| Component    | Description                |
|-------------|----------------------------|
| position    | Position 2D                |
| velocity    | Vitesse de déplacement     |
| drawable    | Ressources graphiques      |
| controllable| Contrôle de l'entité       |

### Initialisation

Dans `loadAssets()` :
```cpp
reg.register_component<component::position>();
reg.register_component<component::velocity>();
reg.register_component<component::drawable>();
reg.register_component<component::controllable>();
```

## Configuration

### Paramètres par défaut
```cpp
fps = 200           // Limite de FPS
baseSpeed = 600.f   // Vitesse de base du vaisseau
str_ip = "127.0.0.1"// IP par défaut
str_port = "8080"   // Port par défaut
```

## Structure des assets

```
ressources/
├── sprites/
│   └── vaisseau{id}.png    # Sprites des vaisseaux
├── background/
│   ├── background.png      # Fond
│   ├── small_stars.png     # Étoiles
│   ├── poudreBleu.png      # Galaxie bleue
│   └── ...                 # Autres éléments d'interface
└── fonts/
    └── NicoMoji.ttf        # Police principale
```

### Ajout de nouveaux assets

1. Placer les fichiers dans le dossier approprié
2. Ajouter dans `loadAssets()` :
```cpp
sprites_game["nouveau_sprite"] = Sprite("../ressources/path/to/sprite.png", true);
drawOrder_game.push_back("nouveau_sprite");
```