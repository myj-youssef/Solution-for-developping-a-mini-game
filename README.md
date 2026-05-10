# Jungle Survival: The Bunker (Survie en Jungle)

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![SFML](https://img.shields.io/badge/SFML-8CC445?style=for-the-badge&logo=sfml&logoColor=white)
![Visual Studio](https://img.shields.io/badge/Visual_Studio-5C2D91?style=for-the-badge&logo=visual%20studio&logoColor=white)

A C++ 2D survival/runner platformer game developed with the **SFML** library for a university Object-Oriented Programming (OOP) mini-project. 

## 📖 Storyline
You play as a tribal guardian tasked with reaching the last refuge before time runs out. The forest is hostile, and every second counts for your survival.

## 🎯 Objectives
- **Survive and Run**: Parcour the perilous jungle and reach the bunker (Treasure) before the 50-second timer expires.
- **Dodge Terrestrial Traps**: Jump over ground-based enemies like Scorpions and landmines.
- **Evade Aerial Threats**: Duck under flying drones and birds.
- **Collect Bananas**: Jump onto platforms to gather bananas and maximize your final score.

## 🕹️ Controls
- `[Space]` or `[Up Arrow]` : **Jump**
- `[Down Arrow]` : **Duck** (slide under aerial obstacles)
- `[Enter]` : **Start Game / Return to Main Menu**
- `[A]` : **About Page / Rules**
- `[Esc]` : **Quit Game**
- `[R]` : **Restart** (from Game Over / Win screen)

## 🛠️ Technical Details & Architecture
This project was designed to strictly apply core Object-Oriented Programming (OOP) principles in C++:
- **SFML Integration**: Handles rendering, graphics, UI elements (health bar, progress bar), and background audio.
- **Polymorphism & Inheritance**: Enemies, traps, and the final treasure inherit from base classes (e.g., `Obstacle`), allowing seamless updates and rendering via a centralized game loop.
- **Manual Memory Management**: Dynamic allocation (`new` / `delete`) via raw pointers is rigorously tracked and cleared to prevent memory leaks during runtime.
- **Resource Optimization**: Textures, fonts, and audio are loaded centrally via the `Game` class and passed by constant reference (`const sf::Texture&`) to instantiated objects, preventing costly redundant I/O operations and excessive memory use.
- **Collision Detection**: Custom bounding box intersection logic triggering score increments or player damage.

## 🚀 How to Run (Visual Studio)

The project includes a Visual Studio solution (`.slnx`) and project file (`.vcxproj`).

1. Make sure you have **SFML (Simple and Fast Multimedia Library)** configured in your Visual Studio C++ environment.
2. Open `Solution for developping a mini game.slnx` or `Survival Runner/Survival Runner.vcxproj`.
3. Verify that the include directories and library dependencies point to your local SFML installation in the project properties.
4. Ensure the `assets/` folder and `.ttf` fonts (`JUNGLEFE.TTF`, `ARIAL.TTF` if applicable) are located in the working directory alongside the executable.
5. Build and Run!

## 👨‍💻 Developers
- **Youssef Myj**
- **Oussama Nagchi**

---
*Developed for S.INFORMATIQUES 2(ème) ANNEE - Programmation Objet C++*