# Breakout 🧱🚀

A robust, highly polished Breakout clone developed in C++ as a technical showcase. This project focuses on clean software architecture, engaging "Game Feel" (Game Juice), and a clear separation between data, logic and rendering.

## 🌟 Key Features & Game Juice

Instead of just building a functional clone, I focused on making the game feel responsive, dynamic, and fun to play:

* **Cinematic Game Flow:** Time scaling (slow-motion) effects upon completing a level or dying, adding dramatic weight to the final hit.
* **Camera Effects (Shake & Zoom):** Dynamic screen shakes trigger when explosive bricks detonate or when losing a life. Additionally, a quick camera zoom effect emphasizes every regular brick destruction.
* **Rich Particle Systems:** Visual feedback is everywhere. Color-matched particles explode from destroyed bricks, falling power-ups leave visual trails, and a specific death effect plays when a ball falls into the dead zone.
* **Dynamic Environment:** The game features a continuously scrolling background texture paired with ambient moving star particles to give life and depth to the scene.
* **Floating Combat Text:** Visual score feedback pops up at the exact location of brick destructions.
* **Advanced Brick Types:** Support for multi-hit bricks, explosive bombs (chain reactions), and indestructible walls.
* **Power-up System:** Random drops including Extra Life, Paddle Enlarge, and Multiball.
* **Session Persistence:** Players can exit a match via the Pause Menu, return to the Main Menu, and smoothly resume their exact game state later using the "Continue" option.
* **High Score Tracking:** The game automatically serializes and saves the player's highest score locally across different play sessions.
* **Flexible Input System:** A custom `PlayerController` maps multiple inputs (like keyboard or gamepad buttons) to specific game actions. 
* **Accessible UI:** Fully implemented Pause Menu and Main Menu with full mouse support for easy navigation.
* **Trajectory Aiming:** A retro-style aiming line to help players plan their initial launch.
* **Dynamic Audio System:** Integrated sound effects with anti-clipping logic (one SFX per frame).
* **Dynamic Paddle Physics:** The ball's exit angle dynamically changes based on the exact impact point on the paddle, giving players precise control over their shots and trajectory.
* **Balanced Power-Ups:** Enhancements like the "Enlarge Paddle" have hard limits to prevent game-breaking scenarios, and all temporary buffs are cleanly reset upon losing a life or progressing to the next level.
* **Adaptive Difficulty:** The ball's velocity increases slightly with every brick destroyed, rewarding precision while naturally scaling the challenge as the level progresses.

## 🏗️ Architecture Highlights

The codebase is structured emphasizing separation of concerns and maintainability:

* **Core Orchestrator (`GameLayer`):** Acts as the root bootstrap of the application. It owns the `GameSession`, instantiates the core systems, and manages global screen fade transitions between different game states.
* **Game Entities:** Pure data containers representing the game world (`GameSession`, `Paddle`, `Brick`, `Ball`, `Level`, `PowerUp`, `Player`). The `GameSession` acts as the single source of truth for the current state of the game.
* **State Machine:** Managed through an `IGameState` interface (`TitleScreen`, `MainMenu`, `Playing`) ensuring clean transitions and isolated logic.
* **Systems vs. Services:**
    * **Systems** (e.g., `PhysicsSystem`, `PaddleSystem`, `GameFlowSystem`): Handle continuous, frame-by-frame updates and logic. The core systems are injected into the `PlayingState` to manipulate the entities.
    * **Services** (e.g., `LevelSetup`, `AudioEngine`): Static utility classes that handle specific operations (like reading a level or playing a sound) and can be accessed globally only when needed.
* **Data-Driven Levels:** Levels are built using simple text arrays in `LevelCatalog.h`. You can design new levels with different layouts, explosive bombs ('A', 'B', 'C'), or indestructible walls ('X') just by modifying the text data, without altering the core C++ logic.
* **Entity Cleanup:** A custom `PerformGarbageCollection` method safely removes destroyed entities (balls, power-ups, bricks) at the end of the frame to keep memory usage clean and prevent game crashes.

## 🎮 Controls

* **Move Paddle:** `Left Arrow` / `Right Arrow` (or `A` / `D`)
* **Launch Ball / Select:** `SPACE`
* **Pause Game:** `ESC`
* **UI Navigation:** `Mouse` (fully supported in menus) or Keyboard
* **Developer Cheat (Skip Level):** `F1` (Useful for quickly reviewing all level designs)

You can build and run the project using **Visual Studio 2022**. Choose the method that best suits you:

### Option A: From Releases (Quickest)
1. Download the source code from the **Releases** tab.
2. Extract the ZIP file and open the `.sln` (Solution) file with Visual Studio 2022.
3. Select your preferred build configuration (**Debug** or **Release**) from the top dropdown menu.
4. Build the solution (`Ctrl + Shift + B`) and run the game!

### Option B: Cloning the Repository
1. Clone the repository to your local machine.
2. Download the `premake5.exe` for Windows from the official Premake website and place it directly in the root folder of the project.
3. Double-click the `GenerateProjects.bat` script. This will use Premake to automatically generate the Visual Studio solution and project files.
4. Open the newly generated `.sln` file in Visual Studio 2022.
5. Select **Debug** or **Release** mode, build the solution (`Ctrl + Shift + B`), and run the game.

## 🛠️ Credits & Third-Party Libraries

This project was built using a custom engine architecture adapted from my personal engine, **Runic2D** (https://github.com/joanmarquesbesses/Runic2D), which was originally developed following the foundational concepts of the **Hazel** engine series by **The Cherno**. The architecture has been significantly modified and tailored specifically for this Breakout implementation.

### Libraries Used:
* **GLFW**: Window management and input handling.
* **GLAD**: OpenGL loader.
* **GLM**: Mathematics library for graphics.
* **miniaudio**: Audio playback engine.
* **stb_image**: Image loading utilities.
* **stb_truetype**: Font rendering and glyph processing.

---

## 🎥 Gallery
![MultiBall](https://github.com/user-attachments/assets/16773adc-faae-42a5-be2c-4ef168a4027d)

![Explosive](https://github.com/user-attachments/assets/9cb05c7b-3f2f-4917-acc0-f85a7b754860)

