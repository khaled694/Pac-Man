# 🎮 Classic Pac-Man Game

A faithful recreation of the classic Pac-Man arcade game built with **Qt Framework** and **C++**.

## 📋 Table of Contents

- [Features](#features)
- [Game Preview](#game-preview)
- [Installation](#installation)
- [How to Play](#how-to-play)
- [Game Mechanics](#game-mechanics)
- [Architecture](#architecture)
- [Controls](#controls)
- [Credits](#credits)

---

## ✨ Features

### Core Gameplay
- **Classic Pac-Man Experience**: Navigate through a maze, collect dots, and avoid ghosts
- **uncomplete Ghost AI**: Four unique ghost personalities with distinct behaviors
  - **Blinky (Red)**: Aggressive direct chaser - targets Pac-Man's exact position
  - **Pinky (Pink)**: Ambusher - targets 4 tiles ahead of Pac-Man
  - **Inky (Cyan)**: Unpredictable - uses complex vector-based targeting
  - **Clyde (Orange)**: Shy - chases when far, retreats when close
- **Power Pellets**: Eat large dots to turn ghosts blue and vulnerable for 8 seconds
- **Teleport Gates**: Wrap around the left and right edges of the maze
- **Score System**: 
  - Small dots: 10 points
  - Power pellets: 50 points
  - Eating ghosts: 200 points each

### Technical Features
- **Smooth 60 FPS Gameplay**: Consistent frame rate using QTimer
- **Grid-Based Movement**: Pixel-perfect collision detection
- **AI Behavior Modes**:
  - **SCATTER**: Ghosts patrol their assigned corners
  - **CHASE**: Ghosts hunt Pac-Man with unique strategies
  - **FRIGHTENED**: Ghosts flee randomly when power-up is active
  - **DEAD**: Ghosts return to spawn and revive
- **Mode Switching**: Automatic alternation between SCATTER and CHASE every 7 seconds
- **Ghost House Logic**: Ghosts start inside and must exit through the gate
- **Visual Feedback**: Animated Pac-Man mouth, color-coded ghosts, score display

---

## 🎬 Game Preview

```
┌─────────────────────────────────────┐
│ SCORE: 420                          │
├─────────────────────────────────────┤
│ ███████████████████████████████████ │
│ █ · · · · · · █ · · · █ · · · · · █ │
│ █ · ███████ · █ · █ · █ · ███████ · █ │
│ █ · █     · · · · █ · · · ·     █ · █ │
│ █ · █ · ██ · █████████ · ██ · █ · █ │
│ █ · · · · · · · · · · · · · · · · · █ │
│ █ · █████ · ████▓████ · █████ · █ │
│   · █   · · █       █ · ·   █ ·   │ ← Teleport
│ █ · █████ · █████████ · █████ · █ │
│ █ · · · · · · · · · · · · · · · · · █ │
│ █ · █ · ██ · █████████ · ██ · █ · █ │
│ █ · █ · · · · · · █ · · · · · · █ · █ │
│ █ · ███████ · █ · █ · █ · ███████ · █ │
│ █ ● · · · · · █ · · · █ · · · · · ● █ │
│ ███████████████████████████████████ │
└─────────────────────────────────────┘

Legend:
  █ = Wall
  · = Small Dot (10 pts)
  ● = Power Pellet (50 pts)
  ▓ = Ghost House Gate
  C = Pac-Man
  👻 = Ghosts (Red, Pink, Cyan, Orange)
```

---

## 🚀 Installation

### Prerequisites

- **Qt Framework** (5.12 or higher)
- **C++ Compiler** (C++11 or higher)
- **qmake** or **CMake**

### Build Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/pacman-qt.git
   cd pacman-qt
   ```

2. **Open the project**
   ```bash
   # Using Qt Creator
   # Open pacman.pro in Qt Creator

   # OR using qmake
   qmake
   make
   ```

3. **Run the game**
   ```bash
   ./pacman
   ```

---

## 🎮 How to Play

### Objective
- Collect all the dots in the maze while avoiding the ghosts
- Use power pellets strategically to turn the tables on the ghosts
- Survive as long as possible and achieve the highest score!

### Game Over Conditions
- **Loss**: Pac-Man collides with a ghost (when not in power-up mode)
- **Win**: Collect all 690 points worth of dots

---

## ⚙️ Game Mechanics

### Movement System
- **Grid-Based**: Characters move on a 64x64 pixel tile grid
- **Speed**: Pac-Man moves at 2 pixels/frame, Ghosts at 1 pixel/frame
- **Collision**: Advanced bounding box detection prevents wall clipping
- **Teleportation**: Exiting the left/right edges warps you to the opposite side

### Ghost AI Behavior

#### Personality Targeting (CHASE Mode)
| Ghost | Color | Personality | Targeting Logic |
|-------|-------|-------------|-----------------|
| Blinky | 🔴 Red | Shadow | Targets Pac-Man's exact current position |
| Pinky | 🩷 Pink | Speedy | Targets 4 tiles ahead of Pac-Man's direction |
| Inky | 🩵 Cyan | Bashful | Targets based on Blinky's position + 2 tiles ahead of Pac-Man (doubled vector) |
| Clyde | 🟠 Orange | Pokey | Chases if >8 tiles away, retreats to corner if closer |

#### Mode System
```
SCATTER (7 sec) → CHASE (7 sec) → SCATTER (7 sec) → ...
       ↓                                    ↓
  Patrol corners                    Hunt Pac-Man

POWER PELLET EATEN → FRIGHTENED (8 sec) → Return to previous mode
                           ↓
                    Random movement
                    Vulnerable to Pac-Man
```

### Power-Up System
- **Duration**: 8 seconds
- **Effect**: Ghosts turn blue and flee randomly
- **Scoring**: Eating a ghost awards 200 points
- **Ghost Respawn**: Eaten ghosts return to the ghost house and revive in SCATTER mode

---

## 🏗️ Architecture

### Class Structure

```
GameManager (Main Game Loop)
    │
    ├── GameWorld (Map & Grid Logic)
    │   ├── Grid[15][19]         // Wall layout
    │   ├── prizeGrid[15][19]    // Dot positions
    │   └── dotItems[15][19]     // Visual dot references
    │
    ├── PacMan (Player Character)
    │   ├── Movement
    │   ├── Collision Detection
    │   ├── Score Tracking
    │   └── Input Handling
    │
    └── Ghost × 4 (AI Enemies)
        ├── AI Personality System
        ├── Mode Management
        ├── Pathfinding
        └── Collision Detection
```

### Key Design Patterns
- **State Pattern**: Ghost mode management (SCATTER, CHASE, FRIGHTENED, DEAD)
- **Observer Pattern**: QTimer signals for game loop and power-up timing
- **Strategy Pattern**: Different AI behaviors per ghost type

### File Structure
```
pacman/
├── main.cpp              // Entry point
├── gamemanager.h/cpp     // Game loop and coordination
├── World.h/cpp           // Map layout and rendering
├── pacMan.h/cpp          // Player logic
├── ghost.h/cpp           // Ghost AI
└── pic/                  // Image assets
    ├── pac.png
    ├── fullpac.png
    ├── red_ghost.png
    ├── pink_ghost.png
    ├── cyan_ghost.png
    ├── orange_ghost.png
    └── blue_ghost.png
```

---

## 🎹 Controls

| Key | Action |
|-----|--------|
| **Arrow Keys** | Move Pac-Man (Up, Down, Left, Right) |
| **WASD** | Alternative movement controls |
| **ESC** | Quit game |

### Movement Notes
- Pac-Man continues in the current direction until hitting a wall or changing direction
- Direction changes are buffered - you can queue the next turn slightly early
- Smooth 60 FPS ensures responsive controls

---

## 🐛 Known Issues & Future Improvements

### Current Limitations
- No lives system (one hit = game over)
- No level progression
- No fruit bonuses
- No sound effects or music

### Planned Features
- [ ] Multiple lives system
- [ ] Level progression with increasing difficulty
- [ ] Bonus fruit items
- [ ] Sound effects and background music
- [ ] High score persistence
- [ ] Pause functionality
- [ ] Menu system

---

## 🛠️ Technical Details

### Dependencies
- **Qt Modules**: QtCore, QtGui, QtWidgets
- **C++ Standard**: C++11 minimum
- **Build System**: qmake or CMake

### Performance
- **Target FPS**: 60
- **Frame Time**: ~16ms per frame
- **Optimizations**: 
  - Decision-making only at tile intersections
  - Efficient grid-based collision detection
  - Visual dot caching (created once, removed on collection)

### Configuration
Edit these constants in the source to customize gameplay:

```cpp
// GameWorld.h
const int TILE_SIZE = 64;     // Pixel size of each grid tile
const int rows = 15;          // Maze height
const int cols = 19;          // Maze width

// Ghost.cpp
int speed = 1;                // Ghost movement speed
int powerUpDuration = 8000;   // Power pellet duration (ms)
int modeToggleDuration = 7000;// SCATTER/CHASE switch time (ms)

// PacMan.cpp
int speed = 2;                // Pac-Man movement speed
```

---

## 📚 Learning Outcomes

This project demonstrates understanding of:

### Programming Concepts
- **Object-Oriented Programming**: Classes, inheritance, encapsulation
- **Design Patterns**: State pattern, Observer pattern, Strategy pattern
- **Event-Driven Programming**: Qt signals and slots, timer events
- **Game Loop Architecture**: Fixed timestep updates, frame-independent movement

### Data Structures & Algorithms
- **2D Arrays**: Grid-based map representation
- **Pathfinding**: Manhattan distance for ghost AI
- **State Machines**: Ghost behavior mode switching
- **Collision Detection**: Bounding box intersection algorithms

### Qt Framework Skills
- **Qt Graphics**: QGraphicsScene, QGraphicsView, QGraphicsPixmapItem
- **Qt Core**: QTimer, QObject, QPointF, QRectF
- **Event Handling**: Keyboard input processing
- **Resource Management**: Proper memory cleanup and parent-child ownership

---

## 🎓 Academic Context

**Course**: Object-Oriented Programming / Game Development  
**Institution**: [Your University Name]  
**Semester**: [Semester/Year]  
**Project Type**: Individual/Team Course Project

### Development Timeline
- **Week 1-2**: Core game architecture and basic movement
- **Week 3-4**: Collision detection and dot collection system
- **Week 5-6**: Ghost AI implementation
- **Week 7**: Power-up system and game over logic
- **Week 8**: Testing, debugging, and polish

---

## 🔧 Development Challenges & Solutions

### Challenge 1: Ghost Oscillation in Hallways
**Problem**: Ghosts were making decisions every frame, causing jittery back-and-forth movement

**Solution**: Implemented tile-based decision tracking - ghosts only recalculate direction when entering a new tile

```cpp
// Track last decision location to prevent over-calculation
int lastDecisionTileX, lastDecisionTileY;
if (currentTileX != lastDecisionTileX || currentTileY != lastDecisionTileY) {
    chooseDirection();  // Only decide on new tiles
}
```

### Challenge 2: Dot Collection Visual Synchronization
**Problem**: Dots appeared to remain visible after collection

**Solution**: Stored pointers to QGraphicsEllipseItem objects in a 2D array for direct removal

```cpp
QGraphicsEllipseItem* dotItems[rows][cols];
// On collection:
scene->removeItem(dotItems[y][x]);
delete dotItems[y][x];
```

### Challenge 3: Ghost Pathfinding
**Problem**: Needed efficient pathfinding without complex algorithms like A*

**Solution**: Used Manhattan distance heuristic - simple, efficient, and perfect for grid-based movement

---

## 📊 Project Statistics

- **Total Lines of Code**: ~1,200
- **Number of Classes**: 4 (GameManager, GameWorld, PacMan, Ghost)
- **Development Time**: ~60 hours
- **Languages Used**: C++ (100%)
- **External Dependencies**: Qt Framework only

---

## 🧪 Testing

### Test Cases Completed
- ✅ Wall collision detection (all angles)
- ✅ Dot collection and score update
- ✅ Power pellet activation and timeout
- ✅ Ghost mode transitions (SCATTER ↔ CHASE)
- ✅ Ghost eaten behavior (respawn logic)
- ✅ Teleport gate functionality
- ✅ Game over trigger on ghost collision
- ✅ All four ghost AI personalities

### Known Bugs
- None critical - game is fully playable

---

## 📖 References

1. **Qt Documentation**: https://doc.qt.io/
2. **Pac-Man Game Design**: "The Pac-Man Dossier" by Jamey Pittman
3. **C++ Reference**: https://en.cppreference.com/
4. **Original Pac-Man (1980)**: Namco/Toru Iwatani

---

## 📜 License

This project was created for educational purposes as part of coursework at [University Name].

**License**: MIT License (Open Source)

---

## 👨‍💻 Author

**Name**: [Your Full Name]  
**Student ID**: [Your ID]  
**Email**: [your.university.email@edu]  
**GitHub**: [github.com/yourusername](https://github.com/yourusername)

### Instructor
**Course Instructor**: [Professor Name]  
**Teaching Assistant**: [TA Name]

---

## 🙏 Acknowledgments

Special thanks to:
- **Professor [Name]** for guidance on OOP concepts and Qt framework
- **Classmates** for collaborative debugging sessions
- **Qt Community** for comprehensive documentation
- **Toru Iwatani** for creating the timeless Pac-Man game design

---

## 📝 Reflections

### What I Learned
- Deep understanding of Qt's graphics framework and event system
- Practical application of design patterns in real software
- Importance of grid-based collision detection for game development
- How to implement AI personalities using simple algorithms
- Effective debugging techniques for complex state machines

### If I Did It Again
- Implement A* pathfinding for more realistic ghost behavior
- Add configuration file for easier gameplay tuning
- Create unit tests for critical game logic
- Implement save/load functionality
- Add animation system for smoother visual effects

---

**Enjoy the game! 🎮👾**

*"This project taught me that even simple games have complex, elegant systems underneath."*

---

## 📋 Table of Contents

- [Features](#features)
- [Game Preview](#game-preview)
- [Installation](#installation)
- [How to Play](#how-to-play)
- [Game Mechanics](#game-mechanics)
- [Architecture](#architecture)
- [Controls](#controls)
- [Credits](#credits)

---

## ✨ Features

### Core Gameplay
- **Classic Pac-Man Experience**: Navigate through a maze, collect dots, and avoid ghosts
- **Smart Ghost AI**: Four unique ghost personalities with distinct behaviors
  - **Blinky (Red)**: Aggressive direct chaser - targets Pac-Man's exact position
  - **Pinky (Pink)**: Ambusher - targets 4 tiles ahead of Pac-Man
  - **Inky (Cyan)**: Unpredictable - uses complex vector-based targeting
  - **Clyde (Orange)**: Shy - chases when far, retreats when close
- **Power Pellets**: Eat large dots to turn ghosts blue and vulnerable for 8 seconds
- **Teleport Gates**: Wrap around the left and right edges of the maze
- **Score System**: 
  - Small dots: 10 points
  - Power pellets: 50 points
  - Eating ghosts: 200 points each

### Technical Features
- **Smooth 60 FPS Gameplay**: Consistent frame rate using QTimer
- **Grid-Based Movement**: Pixel-perfect collision detection
- **AI Behavior Modes**:
  - **SCATTER**: Ghosts patrol their assigned corners
  - **CHASE**: Ghosts hunt Pac-Man with unique strategies
  - **FRIGHTENED**: Ghosts flee randomly when power-up is active
  - **DEAD**: Ghosts return to spawn and revive
- **Mode Switching**: Automatic alternation between SCATTER and CHASE every 7 seconds
- **Ghost House Logic**: Ghosts start inside and must exit through the gate
- **Visual Feedback**: Animated Pac-Man mouth, color-coded ghosts, score display

---

## 🎬 Game Preview

```
┌─────────────────────────────────────┐
│ SCORE: 420                          │
├─────────────────────────────────────┤
│ ███████████████████████████████████ │
│ █ · · · · · · █ · · · █ · · · · · █ │
│ █ · ███████ · █ · █ · █ · ███████ · █ │
│ █ · █     · · · · █ · · · ·     █ · █ │
│ █ · █ · ██ · █████████ · ██ · █ · █ │
│ █ · · · · · · · · · · · · · · · · · █ │
│ █ · █████ · ████▓████ · █████ · █ │
│   · █   · · █       █ · ·   █ ·   │ ← Teleport
│ █ · █████ · █████████ · █████ · █ │
│ █ · · · · · · · · · · · · · · · · · █ │
│ █ · █ · ██ · █████████ · ██ · █ · █ │
│ █ · █ · · · · · · █ · · · · · · █ · █ │
│ █ · ███████ · █ · █ · █ · ███████ · █ │
│ █ ● · · · · · █ · · · █ · · · · · ● █ │
│ ███████████████████████████████████ │
└─────────────────────────────────────┘

Legend:
  █ = Wall
  · = Small Dot (10 pts)
  ● = Power Pellet (50 pts)
  ▓ = Ghost House Gate
  C = Pac-Man
  👻 = Ghosts (Red, Pink, Cyan, Orange)
```

---

## 🚀 Installation

### Prerequisites

- **Qt Framework** (5.12 or higher)
- **C++ Compiler** (C++11 or higher)
- **qmake** or **CMake**

### Build Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/pacman-qt.git
   cd pacman-qt
   ```

2. **Open the project**
   ```bash
   # Using Qt Creator
   # Open pacman.pro in Qt Creator

   # OR using qmake
   qmake
   make
   ```

3. **Run the game**
   ```bash
   ./pacman
   ```

---

## 🎮 How to Play

### Objective
- Collect all the dots in the maze while avoiding the ghosts
- Use power pellets strategically to turn the tables on the ghosts
- Survive as long as possible and achieve the highest score!

### Game Over Conditions
- **Loss**: Pac-Man collides with a ghost (when not in power-up mode)
- **Win**: Collect all 690 points worth of dots

---

## ⚙️ Game Mechanics

### Movement System
- **Grid-Based**: Characters move on a 64x64 pixel tile grid
- **Speed**: Pac-Man moves at 2 pixels/frame, Ghosts at 1 pixel/frame
- **Collision**: Advanced bounding box detection prevents wall clipping
- **Teleportation**: Exiting the left/right edges warps you to the opposite side

### Ghost AI Behavior

#### Personality Targeting (CHASE Mode)
| Ghost | Color | Personality | Targeting Logic |
|-------|-------|-------------|-----------------|
| Blinky | 🔴 Red | Shadow | Targets Pac-Man's exact current position |
| Pinky | 🩷 Pink | Speedy | Targets 4 tiles ahead of Pac-Man's direction |
| Inky | 🩵 Cyan | Bashful | Targets based on Blinky's position + 2 tiles ahead of Pac-Man (doubled vector) |
| Clyde | 🟠 Orange | Pokey | Chases if >8 tiles away, retreats to corner if closer |

#### Mode System
```
SCATTER (7 sec) → CHASE (7 sec) → SCATTER (7 sec) → ...
       ↓                                    ↓
  Patrol corners                    Hunt Pac-Man

POWER PELLET EATEN → FRIGHTENED (8 sec) → Return to previous mode
                           ↓
                    Random movement
                    Vulnerable to Pac-Man
```

### Power-Up System
- **Duration**: 8 seconds
- **Effect**: Ghosts turn blue and flee randomly
- **Scoring**: Eating a ghost awards 200 points
- **Ghost Respawn**: Eaten ghosts return to the ghost house and revive in SCATTER mode

---

## 🏗️ Architecture

### Class Structure

```
GameManager (Main Game Loop)
    │
    ├── GameWorld (Map & Grid Logic)
    │   ├── Grid[15][19]         // Wall layout
    │   ├── prizeGrid[15][19]    // Dot positions
    │   └── dotItems[15][19]     // Visual dot references
    │
    ├── PacMan (Player Character)
    │   ├── Movement
    │   ├── Collision Detection
    │   ├── Score Tracking
    │   └── Input Handling
    │
    └── Ghost × 4 (AI Enemies)
        ├── AI Personality System
        ├── Mode Management
        ├── Pathfinding
        └── Collision Detection
```

### Key Design Patterns
- **State Pattern**: Ghost mode management (SCATTER, CHASE, FRIGHTENED, DEAD)
- **Observer Pattern**: QTimer signals for game loop and power-up timing
- **Strategy Pattern**: Different AI behaviors per ghost type

### File Structure
```
pacman/
├── main.cpp              // Entry point
├── gamemanager.h/cpp     // Game loop and coordination
├── World.h/cpp           // Map layout and rendering
├── pacMan.h/cpp          // Player logic
├── ghost.h/cpp           // Ghost AI
└── pic/                  // Image assets
    ├── pac.png
    ├── fullpac.png
    ├── red_ghost.png
    ├── pink_ghost.png
    ├── cyan_ghost.png
    ├── orange_ghost.png
    └── blue_ghost.png
```

---

## 🎹 Controls

| Key | Action |
|-----|--------|
| **Arrow Keys** | Move Pac-Man (Up, Down, Left, Right) |
| **WASD** | Alternative movement controls |
| **ESC** | Quit game |

### Movement Notes
- Pac-Man continues in the current direction until hitting a wall or changing direction
- Direction changes are buffered - you can queue the next turn slightly early
- Smooth 60 FPS ensures responsive controls

---

## 🐛 Known Issues & Future Improvements

### Current Limitations
- No lives system (one hit = game over)
- No level progression
- No fruit bonuses
- No sound effects or music

### Planned Features
- [ ] Multiple lives system
- [ ] Level progression with increasing difficulty
- [ ] Bonus fruit items
- [ ] Sound effects and background music
- [ ] High score persistence
- [ ] Pause functionality
- [ ] Menu system

---

## 🛠️ Technical Details

### Dependencies
- **Qt Modules**: QtCore, QtGui, QtWidgets
- **C++ Standard**: C++11 minimum
- **Build System**: qmake or CMake

### Performance
- **Target FPS**: 60
- **Frame Time**: ~16ms per frame
- **Optimizations**: 
  - Decision-making only at tile intersections
  - Efficient grid-based collision detection
  - Visual dot caching (created once, removed on collection)

### Configuration
Edit these constants in the source to customize gameplay:

```cpp
// GameWorld.h
const int TILE_SIZE = 64;     // Pixel size of each grid tile
const int rows = 15;          // Maze height
const int cols = 19;          // Maze width

// Ghost.cpp
int speed = 1;                // Ghost movement speed
int powerUpDuration = 8000;   // Power pellet duration (ms)
int modeToggleDuration = 7000;// SCATTER/CHASE switch time (ms)

// PacMan.cpp
int speed = 2;                // Pac-Man movement speed
```

---

## 📜 License

This project is licensed under the **MIT License** - see the LICENSE file for details.

---

## 👨‍💻 Credits

**Developer**: Your Name  
**Framework**: Qt Framework  
**Inspiration**: Namco's original Pac-Man (1980)

### Acknowledgments
- Ghost AI behavior based on the original Pac-Man game design by Toru Iwatani
- Thanks to the Qt community for excellent documentation and resources

---

## 📞 Contact & Support

- **Issues**: [GitHub Issues](https://github.com/khaled694/pacman-qt/issues)
- **Email**: khaledazdarwesh@gmail.com
- **Contributions**: Pull requests are welcome!

---

## 🎯 Tips & Strategies

1. **Corner Clearing**: Clear corners during SCATTER mode when ghosts are less aggressive
2. **Power Pellet Timing**: Save power pellets for when multiple ghosts are nearby
3. **Clyde Exploitation**: Use Clyde's "shy" behavior - get close to make him retreat
4. **Teleport Escapes**: Use side tunnels to quickly evade pursuing ghosts
5. **Pattern Recognition**: Learn ghost patrol patterns in SCATTER mode

---

**Enjoy the game! 🎮👾**

*Waka waka waka...*
