# Documentation Serveur R-Type

## Architecture Générale

Le serveur R-Type est construit sur une architecture basée sur Boost.Asio avec les caractéristiques suivantes :
- Serveur UDP pour la communication en temps réel
- Système de composants (ECS) pour la gestion du jeu 
- Médiateur pour la gestion des événements
- Spawn system pour la génération d'ennemis

## Composants du Serveur

Le serveur intègre les composants suivants :
```cpp
component::position    // Position des entités
component::velocity   // Vitesse des entités
component::drawable   // Ressources graphiques
component::controllable // Contrôle des entités
component::health    // Points de vie
component::damage    // Dégâts infligés
```

## Protocole de Communication

### Actions du Jeu
```cpp
enum GameAction {
    NONE,
    SHOOT,
    MOVE,
    DOWN,
    LEFT,
    RIGHT,
    JUMP,
    CROUCH,
    POWER_UP,
    SHIELD,
    RESPAWN,
    CONNECT,
    DISCONNECT,
    QUIT
};
```

### Format des Messages
Les messages sont encodés en binaire sur 5 bits puis convertis en string :
```cpp
// Exemple d'encodage
"00001" // SHOOT
"00010" // MOVE
// etc...
```

### Structure des messages
```
[Action binaire] [Arguments...]
```

## Gestion des Clients

### Connexion Client
```cpp
void Server::handle_connect(const boost::asio::ip::udp::endpoint& client) {
    // 1. Création des informations client
    // 2. Spawn d'une entité joueur
    // 3. Envoi de confirmation
    // 4. Broadcast aux autres clients
}
```

### Déconnexion Client
```cpp
void Server::handle_disconnect(const boost::asio::ip::udp::endpoint& client) {
    // Suppression du client et notification
}
```

## Système de Spawn

Le serveur implémente un système de spawn automatique pour les ennemis :

### Types d'Ennemis
1. Type 0 (Tank):
   - Santé: 300
   - Dégâts: 10
   - Vitesse: 5

2. Type 1 (Rapide):
   - Santé: 100
   - Dégâts: 40
   - Vitesse: 10

### Configuration du Spawn
- Timer de spawn: 10 secondes
- Position: Aléatoire sur l'axe Y (0-900)
- Position X fixe: 1800

## Système de Médiation

Le médiateur gère la communication entre les différents systèmes :

### Communication Client → Serveur
```cpp
Mediator::notify(Sender::CLIENT, action, params, registry)
```

Actions gérées :
- CONNECT : Connexion d'un nouveau client
- DISCONNECT : Déconnexion d'un client
- MOVE : Déplacement d'un joueur

## Démarrage du Serveur

Pour lancer le serveur :
```bash
./r-type_server <port>
```

### Initialisation
1. Enregistrement des composants
2. Démarrage du thread de réception
3. Configuration du timer de spawn
4. Boucle principale du serveur

### Configuration requise
- Port UDP ouvert
- Support ANSI pour Windows (activé automatiquement)
- Boost.Asio

## Logs et Debug

Le serveur utilise un système de couleurs pour les logs :
- VERT : Connexions et succès
- ROUGE : Déconnexions et erreurs
- BLEU : Actions des joueurs
- JAUNE : Informations système

## Extension du Serveur

Pour ajouter de nouvelles fonctionnalités :

1. Nouveaux types d'ennemis :
```cpp
void Server::spawn_mob(int mob_type) {
    // Ajouter de nouveaux cas dans le switch
}
```

2. Nouvelles actions :
```cpp
// 1. Ajouter dans GameAction
// 2. Mettre à jour get_action_name()
// 3. Implémenter le handler dans le médiateur
```

3. Nouveaux composants :
```cpp
// 1. Créer le composant
// 2. Enregistrer dans Server::start()
reg.register_component<component::nouveau_composant>();
```