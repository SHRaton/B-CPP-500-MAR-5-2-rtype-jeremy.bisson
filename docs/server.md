# R-Type Server Documentation 🖥️

## General Architecture 🏗️

The R-Type server is built on a Boost.Asio architecture with the following features:
- UDP server for real-time communication
- Component system (ECS) for game management
- Mediator for event handling
- Spawn system for enemy generation

## Server Components 🧩

The server integrates the following components:
```cpp
component::position    // Entity positions
component::velocity   // Entity speeds
component::drawable   // Graphical resources
component::controllable // Entity control
component::health    // Hit points
component::damage    // Damage dealt
```

## Communication Protocol 📡

### Game Actions 🎮
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

### Message Format 📝
Messages are encoded in binary on 5 bits then converted to string:
```cpp
// Encoding example
"00001" // SHOOT
"00010" // MOVE
// etc...
```

### Message Structure
```
[Binary Action] [Arguments...]
```

## Client Management 👥

### Client Connection
```cpp
void Server::handle_connect(const boost::asio::ip::udp::endpoint& client) {
    // 1. Create client information
    // 2. Spawn player entity
    // 3. Send confirmation
    // 4. Broadcast to other clients
}
```

### Client Disconnection
```cpp
void Server::handle_disconnect(const boost::asio::ip::udp::endpoint& client) {
    // Remove client and notify
}
```

## Spawn System 👾

The server implements an automatic spawn system for enemies:

### Enemy Types 🤖
1. Type 0 (Tank):
   - Health: 300
   - Damage: 10
   - Speed: 5

2. Type 1 (Fast):
   - Health: 100
   - Damage: 40
   - Speed: 10

### Spawn Configuration ⚙️
- Spawn timer: 10 seconds
- Position: Random on Y axis (0-900)
- Fixed X position: 1800

## Mediation System 🔄

The mediator manages communication between different systems:

### Client → Server Communication
```cpp
Mediator::notify(Sender::CLIENT, action, params, registry)
```

Handled actions:
- CONNECT: New client connection
- DISCONNECT: Client disconnection
- MOVE: Player movement

## Server Startup 🚀

To launch the server:
```bash
./r-type_server <port>
```

### Initialization
1. Component registration
2. Reception thread startup
3. Spawn timer configuration
4. Server main loop

### Requirements ✅
- Open UDP port
- ANSI support for Windows (automatically enabled)
- Boost.Asio

## Logs and Debug 📊

The server uses a color system for logs:
- GREEN: Connections and successes
- RED: Disconnections and errors
- BLUE: Player actions
- YELLOW: System information

## Server Extension 🔧

To add new features:

1. New enemy types:
```cpp
void Server::spawn_mob(int mob_type) {
    // Add new cases in switch
}
```

2. New actions:
```cpp
// 1. Add to GameAction
// 2. Update get_action_name()
// 3. Implement handler in mediator
```

3. New components:
```cpp
// 1. Create component
// 2. Register in Server::start()
reg.register_component<component::new_component>();
```