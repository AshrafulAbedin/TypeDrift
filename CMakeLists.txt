cmake_minimum_required(VERSION 3.10)

project(TypeDrift LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TypeDrift
    src/main.cpp
    src/auth/user_manager.cpp
    src/auth/hashing.cpp
    src/game_modes/SpeedTest.cpp
    src/game_modes/TerminalSetup.cpp
    src/utils/user.cpp 
    src/utils/file_helper.cpp
    src/core/session_logger.cpp
    src/core/leaderboard.cpp
)
target_include_directories(TypeDrift PRIVATE
    src
    src/auth 
    src/game_modes 
    src/utils
    src/core
)