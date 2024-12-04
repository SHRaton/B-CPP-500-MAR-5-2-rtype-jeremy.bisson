# R-Type Client Documentation 🎮

## Overview 👁️

The Client is the player program that connects to the server, creates/joins a game instance, and displays the game.

## Architecture 🏗️

The client is divided into 3 main parts:
- Game display (Core and GUI)
- Connection management/server communication (UDPNetworkClient)
- Entity and component system (Registry)

## Game Display 🖥️

### File Structure
```
src/client/
├── Core.cpp       # Main game management
└── GuiGame.cpp    # Game graphical interface
```

### Main Features ⭐

SFML library is used for game display. The client has several graphical interfaces:
1. Main menu
2. Connection screen
3. Game interface

#### Core Class 🎯

The `Core` class manages:
- Resource loading via `loadAssets()`
- SFML window initialization
- Sprite management and display order
- Component system via Registry

#### Sprite Collections 🎨

```cpp
sprites_menu     // Main menu sprites
sprites_login    // Login screen sprites
sprites_game     // Game sprites
```

#### Main Loop 🔄

The `gui_game()` function manages:
- Keyboard events
- Player position updates
- Background sprite display
- Player ship and other players display
- FPS and latency information display

## Movement Management 🕹️

The `handleMoove()` function implements:

### Controls
- Directional keys for movement
- Ship sprite animation
- Sending new positions to server via UDP

### Movement Code Example
```cpp
if (keysPressed[sf::Keyboard::Up]) {
    movement.y -= baseSpeed * deltaSeconds;
    // Animation logic...
}
```

## Network Communication 📡

### UDPNetworkClient Class

Features:
- UDP connection with server
- Message queues
- Dedicated network operations thread

### Protocol

Main messages:
```
MOVE id x y     # Player position update
CONNECT id      # New player connection
```

## Entity System (ECS) 🧩

### Available Components

| Component    | Description                |
|-------------|----------------------------|
| position    | 2D Position                |
| velocity    | Movement speed             |
| drawable    | Graphical resources        |
| controllable| Entity control             |

### Initialization

In `loadAssets()`:
```cpp
reg.register_component<component::position>();
reg.register_component<component::velocity>();
reg.register_component<component::drawable>();
reg.register_component<component::controllable>();
```

## Configuration ⚙️

### Default Parameters
```cpp
fps = 200           // FPS limit
baseSpeed = 600.f   // Base ship speed
str_ip = "127.0.0.1"// Default IP
str_port = "8080"   // Default port
```

## Asset Structure 📁

```
resources/
├── sprites/
│   └── ship{id}.png       # Ship sprites
├── background/
│   ├── background.png     # Background
│   ├── small_stars.png    # Stars
│   ├── blueNebula.png    # Blue galaxy
│   └── ...               # Other interface elements
└── fonts/
    └── NicoMoji.ttf      # Main font
```

### Adding New Assets 🎨

1. Place files in appropriate folder
2. Add in `loadAssets()`:
```cpp
sprites_game["new_sprite"] = Sprite("../resources/path/to/sprite.png", true);
drawOrder_game.push_back("new_sprite");
```