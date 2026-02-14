#include "leaderboard.h"
#include "file_helper.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "hashing.h"

std::string Leaderboard::getDifficultyString(int difficulty) {
    if (difficulty == 1) return "Easy";
    if (difficulty == 2) return "Medium";
    if (difficulty == 3) return "Hard";
    return "Easy";
}

std::string Leaderboard::getLeaderboardFilePath(int difficulty) {
    if (difficulty == 1) return "../data/leaderboards/easy.txt";
    if (difficulty == 2) return "../data/leaderboards/medium.txt";
    if (difficulty == 3) return "../data/leaderboards/hard.txt";
    return "../data/leaderboards/easy.txt";
}

std::vector<Leaderboard::LeaderboardEntry> Leaderboard::parseLeaderboardFile(const std::string& filepath) {
    std::vector<LeaderboardEntry> entries;
    std::vector<std::string> lines = FileHandler::readLines(filepath);

    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty()) continue;

        LeaderboardEntry entry = {"", 0, 0, 0.0f};

        // Format: userId||wpm||accuracy||score
        // Find "||" delimiters
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

bool Leaderboard::submitScore(const std::string& userId, int difficulty, int wpm, int accuracy) {
    // Ensure directories exist
    if (!FileHandler::directoryExists("../data")) {
        FileHandler::createDirectory("../data");
    }
    if (!FileHandler::directoryExists("../data/leaderboards")) {
        FileHandler::createDirectory("../data/leaderboards");
    }

    std::string filepath = getLeaderboardFilePath(difficulty);

    // Calculate composite score
    float score = (float)wpm * (float)accuracy / 100.0f;

    // Load existing leaderboard
    std::vector<LeaderboardEntry> entries = parseLeaderboardFile(filepath);

    // Check if it qualifies (list not full, or beats the lowest)
    if ((int)entries.size() >= MAX_ENTRIES && !entries.empty()) {
        if (score <= entries[entries.size() - 1].score) {
            return false;  // doesn't qualify
        }
    }

    // Create new entry
    LeaderboardEntry newEntry;
    newEntry.userId = userId;
    newEntry.wpm = wpm;
    newEntry.accuracy = accuracy;
    newEntry.score = score;

    // Find insertion point (sorted descending by score)
    int insertPos = (int)entries.size();
    for (int i = 0; i < (int)entries.size(); i++) {
        if (score > entries[i].score) {
            insertPos = i;
            break;
        }
    }

    // Insert at the correct position
    entries.insert(entries.begin() + insertPos, newEntry);

    // Trim to MAX_ENTRIES
    if ((int)entries.size() > MAX_ENTRIES) {
        entries.resize(MAX_ENTRIES);
    }

    // Write back
    return writeLeaderboardFile(filepath, entries);
}

std::vector<Leaderboard::LeaderboardEntry> Leaderboard::loadLeaderboard(int difficulty) {
    std::string filepath = getLeaderboardFilePath(difficulty);
    return parseLeaderboardFile(filepath);
}

void Leaderboard::displayLeaderboard(int difficulty) {
    std::string diffStr = getDifficultyString(difficulty);
    std::vector<LeaderboardEntry> entries = loadLeaderboard(difficulty);

    std::cout << "\n=== " << diffStr << " LEADERBOARD ===" << std::endl;

    if (entries.empty()) {
        std::cout << "No entries yet." << std::endl;
        return;
    }

    std::cout << std::left
              << std::setw(6) << "Rank"
              << std::setw(26) << "User"
              << std::setw(8) << "WPM"
              << std::setw(10) << "Accuracy"
              << std::setw(10) << "Score"
              << std::endl;

    std::cout << std::string(60, '-') << std::endl;

    int displayCount = (int)entries.size();
    if (displayCount > 100) displayCount = 100;  // Show top 100 on screen
    initCharMap();
    for (int i = 0; i < displayCount; i++) {
        std::cout << std::left
                  << std::setw(6) << (i + 1)
                  << std::setw(26) << decryptString(entries[i].userId)
                  << std::setw(8) << entries[i].wpm
                  << std::setw(10) << entries[i].accuracy
                  << std::fixed << std::setprecision(2)
                  << std::setw(10) << entries[i].score
                  << std::endl;
    }

    if ((int)entries.size() > 20) {
        std::cout << "... and " << (entries.size() - 20) << " more entries." << std::endl;
    }

    std::cout << "==============================" << std::endl;
}
