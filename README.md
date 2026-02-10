# TypeDrift 
![TypeDrift Header](https://media0.giphy.com/media/v1.Y2lkPTc5MGI3NjExcmIyeWQ2Y3kyZDMxa3NzODVxdDRlenkxajZnYjE1NTgza29iZWp3diZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/Er3QVX48nt5ok/giphy.gif)

## Overview
**TypeDrift** is a high-performance, terminal-based typing speed and accuracy trainer developed in **C++**. Designed for developers and terminal enthusiasts, it focuses on providing a distraction-free environment to improve typing ergonomics and words-per-minute (WPM) metrics.

---

## Course Information

**Course Title:** Software Project Lab-I  
**Course Code:** SWE 4304  
**Department:** Computer Science & Engineering  
**Program:** B.Sc. in Software Engineering

---

## Technical Highlights 
This project was built focusing on modern software engineering practices:

* **Object-Oriented Programming (OOP):** Deep implementation of Composition, Encapsulation, and modular class design.
* **Security & Hashing:** Developed a custom user authentication system using secure hashing algorithms to protect user data.
* **Data Persistence:** Efficient file handling for saving user profiles, session history, and leaderboards.
* **Modern Build Systems:** Cross-platform compatibility managed via **CMake**.
* **Linux/Windows Optimized:** Designed to run seamlessly in Windows Git Bash (MINGW64) and Linux environments.

---

## Key Features

- [x] **Real-time WPM Tracking:** Live calculation of speed and accuracy.
- [x] **Secure User Login:** Persistent user profiles with hashed username & password protection.
- [x] **Performance History:** View your growth over time with saved session statistics.
- [x] **Dynamic Word Engine:** Randomized text generation to prevent muscle memory repetition.
- [x] **Fun Modes:** Challenges are given to players with various difficulty modes.

---

## Installation & Build

### Prerequisites

* C++17 compatible compiler (GCC, Clang, or MSVC)
* CMake 3.10 or higher

### Build Instructions

**1. Clone the repository:**
```bash
git clone https://github.com/AshrafulAbedin/TypeDrift.git
cd TypeDrift
```

**2. Configure and build:**
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

**3. Run the application:**
```bash
# Linux / macOS
./TypeDrift

# Windows (Git Bash / MINGW64)
.\TypeDrift.exe
```

---

## License

This project is licensed under the **MIT License**.  
You are free to use, modify, and distribute this software with proper attribution.  
See the [LICENSE](LICENSE) file for full details.
