# TypeDrift

> A high-performance, offline-first CLI typing trainer built in C++ for developers who never leave the terminal.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Demo](#demo)
- [Tech Stack](#tech-stack)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building from Source](#building-from-source)
  - [Running TypeDrift](#running-typedrift)
- [Game Modes](#game-modes)
- [Project Structure](#project-structure)
- [Team](#team)
- [Future Roadmap](#future-roadmap)
- [License](#license)

---

## Overview

TypeDrift is a lightweight, distraction-free typing trainer that runs entirely in your terminal — no browser, no internet, no bloat.

Most typing tools are designed around graphical interfaces that feel foreign to developers. TypeDrift is different. It was built from the ground up for people who already spend their day in the terminal, making the practice environment identical to the one they actually work in. All session data is stored locally on your machine, your credentials are protected with custom cryptographic hashing, and the TUI is designed to stay out of your way.

**Why TypeDrift?**

- **Zero latency** — No web overhead. Keystrokes are processed at the OS level.
- **Fully offline** — No internet connection required, ever.
- **Privacy-first** — All data lives on your machine.
- **Terminal-native** — Built for Linux and macOS CLI environments.

---

## Features

### Real-Time Performance Engine
Live WPM and accuracy calculated continuously throughout each session, delivered without interrupting your typing flow.

### Color-Coded Visual Feedback
- **Grey** — characters not yet typed  
- **Green** — correct keystrokes  
- **Red** — incorrect keystrokes  

Instant, at-a-glance feedback with no extra UI clutter.

### Secure Local Authentication
Persistent user profiles protected with custom cryptographic password hashing. Your credentials never leave your machine.

### Performance History & Data Persistence
Session results are saved after every completed run. Revisit your past scores anytime to track long-term improvement.

### Dynamic Word Engine
Randomized text passages generated each session to ensure every practice run is a fresh challenge.

### Local Leaderboard
High scores are stored persistently on disk, supporting healthy competition among multiple users sharing the same system.

### Multiple Difficulty Tiers & Game Modes
Pre-configured difficulty levels covering beginner to advanced, plus specialized challenge modes to keep training varied and engaging.

---

## Demo

```
┌─────────────────────────────────────────────────────┐
│  TypeDrift  │  WPM: 87  │  Accuracy: 94%  │  00:42  │
├─────────────────────────────────────────────────────┤
│                                                     │
│  the quick brown fox jumps over the lazy dog        │
│  ███████████████████████░░░░░░░░░░░░░░░░░░░         │
│                                                     │
│  [green: correct]  [red: incorrect]  [grey: pending]│
└─────────────────────────────────────────────────────┘
```

---

## Tech Stack

| Category              | Tool / Technology              |
|-----------------------|--------------------------------|
| Language              | C++ (C++17 Standard)           |
| Build System          | CMake & Makefile               |
| IDE                   | Visual Studio Code             |
| Version Control       | Git                            |
| Collaboration         | GitHub                         |
| Target OS             | Linux & macOS (Unix-based)     |

---

## Getting Started

### Prerequisites

Ensure the following are installed on your system:

- A Unix-based OS (Linux or macOS)
- `g++` with C++17 support (`g++ --version`)
- `cmake` ≥ 3.10 (`cmake --version`)
- `make`

#Downloading the Repository

```bash
# Clone the repository
git clone https://github.com/AshrafulAbedin/TypeDrift.git
cd TypeDrift
```

### Running TypeDrift

```bash
# Run the executable
cd code/build
# From the build directory
./TypeDrift
```

### Building from Source

```bash
#If there is problem with given executable, you can build the project from source using the following commands:
rm -rf build
# Create a build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Compile
cmake --build .

# Run the executable
./TypeDrift
```



On first launch, you will be prompted to create a user profile. Your profile and session history will be saved locally for future sessions.

---

## Game Modes

| Mode            | Description                                                              |
|-----------------|--------------------------------------------------------------------------|
| **Classic**     | Standard timed typing test. Type the passage as fast and accurately as you can. |
| **Time Control**| Race against a countdown timer. Every correct word buys you more time.   |
| **No-Vowel**    | Words are displayed without vowels — you must mentally reconstruct them as you type. |
| **Falling Words**| Words appear and scroll down the screen. Type them before they reach the bottom. |

---

## Project Structure


```
TypeDrift/
├── gitignore
├── CMakeLists.txt
├── 📂 data/
│   ├── 📂 leaderboards/
│   │   ├── 📂 career/
│   │   │   └── 📄 easy.txt
│   │   └── 📂 funmode/
│   │   │   ├── 📄 FallingWords.txt
│   │   │   ├── 📄 TimeTest.txt
│   │   │   └── 📄 VowelGame.txt
│   ├── 📂 texts/
│   │   ├── 📂 easy/
│   │   │   ├── 📄 easy1-50.txt
│   │   ├── 📂 hard/
│   │   │   ├── 📄 hard1-50.txt
│   │   ├── 📂 medium/
│   │   │   ├── 📄 medium1-50.txt
│   │   ├── 📂 no_vowel_txt/
│   │   │   ├── 📄 n_vwl1-50.txt
│   │   └── 📂 time_txt/
│   │   │   ├── 📄 time1-50.txt
│   ├── 📂 users/
│   └── 📄 users.txt
├── 📂 doc/
│   ├── 📕 progress-presentation.pdf
│   ├── 📕 proposal-presentation.pdf
│   └── 📕 proposal-report.pdf
├── 📖 **README.md**
└── 📁 src/
│   ├── 📂 auth/
│   │   ├── 📄 hashing.cpp
│   │   ├── 📄 hashing.h
│   │   ├── 📄 user_manager.cpp
│   │   └── 📄 user_manager.h
│   ├── 📂 core/
│   │   ├── 📄 leaderboard.cpp
│   │   ├── 📄 leaderboard.h
│   │   ├── 📄 session_logger.cpp
│   │   └── 📄 session_logger.h
│   ├── 📂 game_modes/
│   │   ├── 📄 falling_words.cpp
│   │   ├── 📄 falling_words.h
│   │   ├── 📄 fw_animation.h
│   │   ├── 📄 fw_common.h
│   │   ├── 📄 fw_database.h
│   │   ├── 📄 fw_score.h
│   │   ├── 📄 SpeedTest.cpp
│   │   ├── 📄 SpeedTest.h
│   │   ├── 📄 TerminalSetup.cpp
│   │   ├── 📄 TerminalSetup.h
│   │   ├── 📄 TimeTest.cpp
│   │   ├── 📄 TimeTest.h
│   │   ├── 📄 vowel_game.cpp
│   │   └── 📄 vowel_game.h
│   ├── 📄 main.cpp
│   └── 🔧 utils/
│   │   ├── 📄 file_helper.cpp
│   │   ├── 📄 file_helper.h
│   │   ├── 📄 user.cpp
│   │   └── 📄 user.h
```

---

## Team

This project was developed as part of **SWE 4304: Software Project Lab I** at the **Islamic University of Technology (IUT)**, **B.Sc. in Software Engineering program**, Department of Computer Science and Engineering.

| Name                    | Student ID   | Contributions                                                   |
|-------------------------|--------------|-----------------------------------------------------------------|
| S. M. Samiul Hossain    | 230042110    | Core game loop, TUI layout, WPM/accuracy engine, text randomization, CMake integration, Time Control mode |
| Md. Ashraful Abedin     | 230042111    | User authentication, password hashing, registration flow, data encryption, No-Vowel mode |
| Shaikh Muhaiminul Hasan | 230042113    | File handling system, multi-user profile management, session history storage, local leaderboard, Falling Words mode |

**Supervisor:** Aashnan Rahman, Junior Lecturer — Department of CSE, IUT

---

## Future Roadmap

- [ ] **Global Leaderboards** — Cloud-based competitive ranking across machines worldwide
- [ ] **Advanced Analytics** — ASCII/Unicode terminal charts for WPM and accuracy trends over time
- [ ] **Adaptive Learning Engine** — ML-driven custom mode that targets your weakest keys
- [ ] **Customizable Themes** — User-defined CLI colors and text styles
- [ ] **Multiplayer Racing Mode** — Real-time socket-based local network typing races
- [ ] **Remote Sync** — Optional cloud backup and cross-device progress sharing

---

## License

This project is currently unlicensed. All rights reserved by the authors.

---
