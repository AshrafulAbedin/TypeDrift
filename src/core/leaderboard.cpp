#include "leaderboard.h"
#include "file_helper.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "hashing.h"
#include "TerminalSetup.h"

std::string Leaderboard::getDifficultyString(int difficulty) {
    if (difficulty == 1) return "Easy";
    if (difficulty == 2) return "Medium";
    if (difficulty == 3) return "Hard";
    return "Easy";
}

// ===================== Career mode paths =====================

std::string Leaderboard::getLeaderboardFilePath(int difficulty) {
    if (difficulty == 1) return FileHandler::getDataPath("leaderboards/career/easy.txt");
    if (difficulty == 2) return FileHandler::getDataPath("leaderboards/career/medium.txt");
    if (difficulty == 3) return FileHandler::getDataPath("leaderboards/career/hard.txt");
    return FileHandler::getDataPath("leaderboards/career/easy.txt");
}

// ===================== Fun mode paths =====================

std::string Leaderboard::getFunLeaderboardFilePath(const std::string& gameName) {
    return FileHandler::getDataPath("leaderboards/funmode/" + gameName + ".txt");
}

std::string Leaderboard::getFunGameDisplayName(const std::string& gameName) {
    if (gameName == "FallingWords") return "Falling Words";
    if (gameName == "TimeTest") return "Timed Test";
    if (gameName == "VowelGame") return "No Vowel Game";
    return gameName;
}

// ===================== Shared parsing =====================

std::vector<Leaderboard::LeaderboardEntry> Leaderboard::parseLeaderboardFile(const std::string& filepath) {
    std::vector<LeaderboardEntry> entries;
    std::vector<std::string> lines = FileHandler::readLines(filepath);

    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty()) continue;

        LeaderboardEntry entry = {"", 0, 0, 0.0f};

        // Format: userId||wpm||accuracy||score
        int pos1 = -1, pos2 = -1, pos3 = -1;
        for (int j = 0; j < (int)lines[i].size() - 1; j++) {
            if (lines[i][j] == '|' && lines[i][j + 1] == '|') {
                if (pos1 == -1) pos1 = j;
                else if (pos2 == -1) pos2 = j;
                else if (pos3 == -1) { pos3 = j; break; }
            }
        }

        if (pos1 == -1 || pos2 == -1 || pos3 == -1) continue;

        entry.userId = lines[i].substr(0, pos1);
        try { entry.wpm = std::stoi(lines[i].substr(pos1 + 2, pos2 - pos1 - 2)); } catch (...) { continue; }
        try { entry.accuracy = std::stoi(lines[i].substr(pos2 + 2, pos3 - pos2 - 2)); } catch (...) { continue; }
        try { entry.score = std::stof(lines[i].substr(pos3 + 2)); } catch (...) { continue; }

        entries.push_back(entry);
    }

    return entries;
}

bool Leaderboard::writeLeaderboardFile(const std::string& filepath, const std::vector<LeaderboardEntry>& entries) {
    std::ostringstream oss;
    for (int i = 0; i < (int)entries.size(); i++) {
        oss << entries[i].userId << "||"
            << entries[i].wpm << "||"
            << entries[i].accuracy << "||"
            << std::fixed << std::setprecision(2) << entries[i].score << "\n";
    }
    return FileHandler::writeFile(filepath, oss.str());
}

// ===================== Career mode submit =====================

bool Leaderboard::submitScore(const std::string& userId, int difficulty, int wpm, int accuracy) {
    // Ensure directories exist
    if (!FileHandler::directoryExists(FileHandler::getDataPath(""))) {
        FileHandler::createDirectory(FileHandler::getDataPath(""));
    }
    if (!FileHandler::directoryExists(FileHandler::getDataPath("leaderboards"))) {
        FileHandler::createDirectory(FileHandler::getDataPath("leaderboards"));
    }
    if (!FileHandler::directoryExists(FileHandler::getDataPath("leaderboards/career"))) {
        FileHandler::createDirectory(FileHandler::getDataPath("leaderboards/career"));
    }

    // Migrate old leaderboard files if they exist
    // Check if old files exist at old location and new dir doesn't have them
    std::string oldPaths[] = {
        FileHandler::getDataPath("leaderboards/easy.txt"),
        FileHandler::getDataPath("leaderboards/medium.txt"),
        FileHandler::getDataPath("leaderboards/hard.txt")
    };
    std::string newPaths[] = {
        FileHandler::getDataPath("leaderboards/career/easy.txt"),
        FileHandler::getDataPath("leaderboards/career/medium.txt"),
        FileHandler::getDataPath("leaderboards/career/hard.txt")
    };
    for (int i = 0; i < 3; i++) {
        if (FileHandler::fileExists(oldPaths[i]) && !FileHandler::fileExists(newPaths[i])) {
            std::string content = FileHandler::readFile(oldPaths[i]);
            if (!content.empty()) {
                FileHandler::writeFile(newPaths[i], content);
            }
        }
    }

    std::string filepath = getLeaderboardFilePath(difficulty);

    float score = (float)wpm * (float)accuracy / 100.0f;

    std::vector<LeaderboardEntry> entries = parseLeaderboardFile(filepath);

    if ((int)entries.size() >= MAX_ENTRIES && !entries.empty()) {
        if (score <= entries[entries.size() - 1].score) {
            return false;
        }
    }

    LeaderboardEntry newEntry;
    newEntry.userId = userId;
    newEntry.wpm = wpm;
    newEntry.accuracy = accuracy;
    newEntry.score = score;

    int insertPos = (int)entries.size();
    for (int i = 0; i < (int)entries.size(); i++) {
        if (score > entries[i].score) {
            insertPos = i;
            break;
        }
    }

    entries.insert(entries.begin() + insertPos, newEntry);

    if ((int)entries.size() > MAX_ENTRIES) {
        entries.resize(MAX_ENTRIES);
    }

    return writeLeaderboardFile(filepath, entries);
}

// ===================== Fun mode submit =====================

bool Leaderboard::submitFunScore(const std::string& userId, const std::string& gameName,
                                  int wpm, int accuracy, int rawScore) {
    // Ensure directories exist
    if (!FileHandler::directoryExists(FileHandler::getDataPath(""))) {
        FileHandler::createDirectory(FileHandler::getDataPath(""));
    }
    if (!FileHandler::directoryExists(FileHandler::getDataPath("leaderboards"))) {
        FileHandler::createDirectory(FileHandler::getDataPath("leaderboards"));
    }
    if (!FileHandler::directoryExists(FileHandler::getDataPath("leaderboards/funmode"))) {
        FileHandler::createDirectory(FileHandler::getDataPath("leaderboards/funmode"));
    }

    std::string filepath = getFunLeaderboardFilePath(gameName);

    // For FallingWords and TimeTest, score = rawScore. For others, score = wpm * accuracy / 100.0
    float score;
    if (gameName == "FallingWords" || gameName == "TimeTest") {
        score = (float)rawScore;
        wpm = rawScore;    // Store score in wpm field for display
        accuracy = 100;    // Not applicable
    } else {
        score = (float)wpm * (float)accuracy / 100.0f;
    }

    std::vector<LeaderboardEntry> entries = parseLeaderboardFile(filepath);

    if ((int)entries.size() >= MAX_ENTRIES && !entries.empty()) {
        if (score <= entries[entries.size() - 1].score) {
            return false;
        }
    }

    LeaderboardEntry newEntry;
    newEntry.userId = userId;
    newEntry.wpm = wpm;
    newEntry.accuracy = accuracy;
    newEntry.score = score;

    int insertPos = (int)entries.size();
    for (int i = 0; i < (int)entries.size(); i++) {
        if (score > entries[i].score) {
            insertPos = i;
            break;
        }
    }

    entries.insert(entries.begin() + insertPos, newEntry);

    if ((int)entries.size() > MAX_ENTRIES) {
        entries.resize(MAX_ENTRIES);
    }

    return writeLeaderboardFile(filepath, entries);
}

// ===================== Career mode display =====================

std::vector<Leaderboard::LeaderboardEntry> Leaderboard::loadLeaderboard(int difficulty) {
    std::string filepath = getLeaderboardFilePath(difficulty);
    return parseLeaderboardFile(filepath);
}

void Leaderboard::displayLeaderboard(int difficulty) {
    std::string diffStr = getDifficultyString(difficulty);
    std::vector<LeaderboardEntry> entries = loadLeaderboard(difficulty);

    std::cout << "\n";
    std::cout << BOLD << CYAN << "    ╔══════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << CYAN << "    ║         " << diffStr << " LEADERBOARD";
    // Pad to fill the box
    int padLen = 32 - (int)diffStr.size();
    for (int i = 0; i < padLen; i++) std::cout << " ";
    std::cout << "║" << RESET << "\n";
    std::cout << BOLD << CYAN << "    ╚══════════════════════════════════════════════════╝" << RESET << "\n\n";

    if (entries.empty()) {
        std::cout << GRAY << "    No entries yet." << RESET << "\n";
        return;
    }

    std::cout << "    " << BOLD << WHITE
              << std::left
              << std::setw(6) << "Rank"
              << std::setw(26) << "User"
              << std::setw(8) << "WPM"
              << std::setw(10) << "Accuracy"
              << std::setw(10) << "Score"
              << RESET << "\n";

    std::cout << "    " << GRAY;
    for (int i = 0; i < 56; i++) std::cout << "─";
    std::cout << RESET << "\n";

    int displayCount = (int)entries.size();
    if (displayCount > 100) displayCount = 100;
    initCharMap();
    for (int i = 0; i < displayCount; i++) {
        std::string rankColor = (i == 0) ? BRIGHT_YELLOW : (i == 1) ? WHITE : (i == 2) ? BRIGHT_CYAN : GRAY;
        std::cout << "    " << rankColor
                  << std::left
                  << std::setw(6) << (i + 1)
                  << std::setw(26) << decryptString(entries[i].userId)
                  << std::setw(8) << entries[i].wpm
                  << std::setw(10) << entries[i].accuracy
                  << std::fixed << std::setprecision(2)
                  << std::setw(10) << entries[i].score
                  << RESET << "\n";
    }

    if ((int)entries.size() > 20) {
        std::cout << GRAY << "    ... and " << (entries.size() - 20) << " more entries." << RESET << "\n";
    }

    std::cout << "\n";
}

// ===================== Fun mode display =====================

std::vector<Leaderboard::LeaderboardEntry> Leaderboard::loadFunLeaderboard(const std::string& gameName) {
    std::string filepath = getFunLeaderboardFilePath(gameName);
    return parseLeaderboardFile(filepath);
}

void Leaderboard::displayFunLeaderboard(const std::string& gameName) {
    std::string displayName = getFunGameDisplayName(gameName);
    std::vector<LeaderboardEntry> entries = loadFunLeaderboard(gameName);

    std::cout << "\n";
    std::cout << BOLD << MAGNETA << "    ╔══════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << MAGNETA << "    ║     " << displayName << " LEADERBOARD";
    int padLen = 36 - (int)displayName.size();
    for (int i = 0; i < padLen; i++) std::cout << " ";
    std::cout << "║" << RESET << "\n";
    std::cout << BOLD << MAGNETA << "    ╚══════════════════════════════════════════════════╝" << RESET << "\n\n";

    if (entries.empty()) {
        std::cout << GRAY << "    No entries yet." << RESET << "\n";
        return;
    }

    bool isFalling = (gameName == "FallingWords");
    bool isTimeTest = (gameName == "TimeTest");

    std::cout << "    " << BOLD << WHITE
              << std::left
              << std::setw(6) << "Rank"
              << std::setw(26) << "User";
    if (isFalling) {
        std::cout << std::setw(10) << "Score";
    } else if (isTimeTest) {
        std::cout << std::setw(12) << "Characters";
    } else {
        std::cout << std::setw(8) << "WPM"
                  << std::setw(10) << "Accuracy"
                  << std::setw(10) << "Score";
    }
    std::cout << RESET << "\n";

    std::cout << "    " << GRAY;
    for (int i = 0; i < 56; i++) std::cout << "─";
    std::cout << RESET << "\n";

    int displayCount = (int)entries.size();
    if (displayCount > 100) displayCount = 100;
    initCharMap();
    for (int i = 0; i < displayCount; i++) {
        std::string rankColor = (i == 0) ? BRIGHT_YELLOW : (i == 1) ? WHITE : (i == 2) ? BRIGHT_CYAN : GRAY;
        std::cout << "    " << rankColor
                  << std::left
                  << std::setw(6) << (i + 1)
                  << std::setw(26) << decryptString(entries[i].userId);
        if (isFalling) {
            std::cout << std::setw(10) << (int)entries[i].score;
        } else if (isTimeTest) {
            std::cout << std::setw(12) << (int)entries[i].score << " chars";
        } else {
            std::cout << std::setw(8) << entries[i].wpm
                      << std::setw(10) << entries[i].accuracy
                      << std::fixed << std::setprecision(2)
                      << std::setw(10) << entries[i].score;
        }
        std::cout << RESET << "\n";
    }

    std::cout << "\n";
}
