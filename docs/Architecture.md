# Architecture R-Type

## Diagramme de Classes

```mermaid
classDiagram
    direction TB

    %% Classes Principales
    class Core {
        -window RenderWindow
        -network UDPNetworkClient*
        -sprites Map
        -player Entity
        +reg registry
        +sys Systems
        +Core()
        +loadAssets()
        +gui()
        +guiMenu()
        +guiGame()
    }

    class UDPNetworkClient {
        -socket udp::socket
        -endpoint udp::endpoint
        +send(message)
        +receive()
        +stop()
    }

    class Sprite {
        -sprite Sprite
        -clock Clock
        +update()
        +setPosition()
        +getSprite()
    }

    class Utils {
        +cat(path)
        +printLog(message)
    }

    %% Classes Serveur
    class ISender {
        <<interface>>
        +handleConnect(context)*
        +handleDisconnect(context)*
        +handleMoves(context)*
        +handleShoot(context)*
    }

    class ServerNetwork {
        -clients Map
        -running bool
        +handleConnect()
        +broadcast()
    }

    class ServerGame {
        -timers List
        +reg registry
        +initTimers()
        +spawnMob()
        +checkCollisions()
    }

    class Mediator {
        +network ISender*
        +game ISender*
        +notify()
        +registerNetwork()
        +registerGame()
    }

    %% Classes ECS
    class Entity {
        -id size_t
        +Entity(value)
        +operator size_t()
    }

    class registry {
        -components Map
        -entities List
        +spawn_entity()
        +kill_entity()
        +register_component()
    }

    class Systems {
        +player_control_system()
        +position_system()
        +draw_system()
    }

    %% Composants
    class health {
        +hp int
    }

    class position {
        +x int
        +y int
    }

    class velocity {
        +vx int
        +vy int
    }

    %% Relations
    Core --> UDPNetworkClient : uses
    Core --> Sprite : manages
    Core --> Utils : uses
    Core --> registry : owns
    Core --> Systems : owns
    Sprite --> Utils : uses

    ServerNetwork ..|> ISender : implements
    ServerGame ..|> ISender : implements
    Mediator --> ISender : owns
    ServerGame --> registry : owns
    ServerNetwork --> Mediator : uses
    ServerGame --> Mediator : uses

    registry --> Entity : manages
    Systems --> registry : uses
    UDPNetworkClient <--> ServerNetwork : UDP
```

## Description de l'Architecture

### Client
- **Core** : Classe principale gérant l'interface utilisateur et la logique client
- **UDPNetworkClient** : Gestion de la communication réseau
- **Sprite** : Gestion des éléments graphiques
- **Utils** : Fonctions utilitaires communes

### Serveur
- **ISender** : Interface pour la communication
- **ServerNetwork** : Gestion des connexions et messages réseau
- **ServerGame** : Logique de jeu et gestion des timers
- **Mediator** : Coordination de la communication

### ECS (Entity Component System)
- **Entity** : Identifiant unique pour chaque élément du jeu
- **registry** : Gestionnaire des entités et composants
- **Systems** : Logique de mise à jour des entités
- **Components** : Composants définissant les propriétés (health, position, velocity)

### Relations Principales
- Communication Client-Serveur via UDP
- ServerNetwork et ServerGame implémentent ISender
- Core utilise registry et Systems pour la logique de jeu
- Mediator coordonne la communication entre Network et Game