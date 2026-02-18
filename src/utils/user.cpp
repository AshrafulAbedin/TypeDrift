#include "user.h"
#include "file_helper.h"
#include "session_logger.h"
#include <sstream>
#include <iomanip> //For setPreacision in serialization & display
#include <iostream>
#include <hashing.h>

User::User() : total_sessions(0), total_wpm(0), best_wpm(0), total_games(0), avg_wpm(0.0f) {}

//User::User(const std::string& name, const std::string& user_id, const std::string& password)
   // : name(name), user_id(user_id), password(password), total_sessions(0), total_wpm(0), best_wpm(0), total_games(0), avg_wpm(0.0f) {}

//  std::string User::getName() const { 
//     HASHING_H::initCharMap();
//     return decryptString(padString(FileHandler::getUserNameFromRegistry(this->user_id), 23)); }
 std::string User::getUserId() const { return user_id; }
// std::string User::getPassword() const { return password; }
int User::getBestWPM() const { return best_wpm; }
int User::getTotalGames() const { return total_games; }
float User::getAvgWPM() const { return avg_wpm; }
void User::setUserId(const std::string& userId){ this->user_id = userId; }

//void User::setName(const std::string& name) { this->name = name; }
//void User::setPassword(const std::string& password) { this->password = password; }

void User::updateStats(int wpm) {
    if (wpm > best_wpm) {
        best_wpm = wpm;
    }
    
    // Update average WPM
    avg_wpm = (avg_wpm * total_games + wpm) / (total_games + 1);
    total_games++;
}

std::string User::serialize() const {
    std::ostringstream oss;
    oss << best_wpm << "||"
        << std::fixed << std::setprecision(2) << avg_wpm << "||"
        << total_games;
    return oss.str();
}

bool User::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string temp;
    
    // Get best_wpm (first token before "||")
    if (!std::getline(iss, temp, '|')) return false;
    try {
        best_wpm = std::stoi(temp);
    } catch (...) {
        return false;
    }
    
    // Skip the second '|' of "||"
    char c;
    iss.get(c);
    if (c != '|') return false;
    
    // Get avg_wpm (second token before next "||")
    if (!std::getline(iss, temp, '|')) return false;
    try {
        avg_wpm = std::stof(temp);
    } catch (...) {
        return false;
    }
    
    // Skip the second '|' of "||"
    iss.get(c);
    if (c != '|') return false;
    
    // Get total_games (remaining content)
    if (!std::getline(iss, temp)) return false;
    try {
        total_games = std::stoi(temp);
    } catch (...) {
        return false;
    }
    
    return true;
}

void User::addSessionResult(int wpm, int accuracy) {
    // total_sessions = User::getTotalSessions();
    total_games++;
    total_wpm += wpm;
    avg_wpm = (float)total_wpm / total_games;
    
    // Update best WPM if needed
    if (wpm > best_wpm) {
        best_wpm = wpm;
    }
}

void User::displaySessionSummary(int wpm, int accuracy) const {
    std::cout << "\n=== SESSION SUMMARY ===\n";
    std::cout << "Current Session WPM: " << wpm << "\n";
    std::cout << "Current Session Accuracy: " << accuracy << "%\n";
    std::cout << "Best WPM: " << best_wpm << "\n";
    std::cout << "Average WPM: " << avg_wpm << "\n";
    std::cout << "Total Sessions: " << total_games<< "\n";
    std::cout << "=====================\n";
}

int User::getTotalSessions() const {
    return total_sessions;
}

int User::getAverageWPM() const {
    return avg_wpm;
}

void User::displayStats() const {
    std::cout << "\n=== View Stats ===\n";
    HASHING_H::initCharMap();
    std::cout << "User ID: " << decryptString(user_id) << "\n\n";

    std::cout << "Choose difficulty to view:\n";
    std::cout << "1. Easy\n2. Medium\n3. Hard\n4. All\n";
    std::cout << "Choice: ";
    int statChoice;
    std::cin >> statChoice;
    std::cin.ignore();

    if (statChoice < 1 || statChoice > 4) {
        std::cout << "Invalid choice.\n";
        return;
    }

    // Read session log file for per-difficulty stats
    std::string filepath = "../data/users/" + user_id + ".txt";
    std::vector<std::string> lines = FileHandler::readLines(filepath);

    if (lines.empty()) {
        std::cout << "\nNo sessions recorded yet.\n";
        return;
    }

    // Format: best_wpm||avg_easy,avg_medium,avg_hard||total_games,easy_count,medium_count,hard_count
    std::string summaryLine = lines[0];
    int bestWpm = 0;
    float avgEasy = 0, avgMedium = 0, avgHard = 0;
    int totalGames = 0, easyCount = 0, mediumCount = 0, hardCount = 0;

    // Parse using "||" as delimiter
    int pos1 = -1;
    for (int i = 0; i < (int)summaryLine.size() - 1; i++) {
        if (summaryLine[i] == '|' && summaryLine[i + 1] == '|') { pos1 = i; break; }
    }
    int pos2 = -1;
    if (pos1 != -1) {
        for (int i = pos1 + 2; i < (int)summaryLine.size() - 1; i++) {
            if (summaryLine[i] == '|' && summaryLine[i + 1] == '|') { pos2 = i; break; }
        }
    }

    if (pos1 != -1 && pos2 != -1) {
        try { bestWpm = std::stoi(summaryLine.substr(0, pos1)); } catch (...) {}

        std::string avgStr = summaryLine.substr(pos1 + 2, pos2 - pos1 - 2);
        int c1 = -1, c2 = -1;
        for (int i = 0; i < (int)avgStr.size(); i++) {
            if (avgStr[i] == ',') { if (c1 == -1) c1 = i; else { c2 = i; break; } }
        }
        if (c1 != -1 && c2 != -1) {
            try { avgEasy = std::stof(avgStr.substr(0, c1)); } catch (...) {}
            try { avgMedium = std::stof(avgStr.substr(c1 + 1, c2 - c1 - 1)); } catch (...) {}
            try { avgHard = std::stof(avgStr.substr(c2 + 1)); } catch (...) {}
        }

        std::string totalStr = summaryLine.substr(pos2 + 2);
        int tc1 = -1, tc2 = -1, tc3 = -1;
        for (int i = 0; i < (int)totalStr.size(); i++) {
            if (totalStr[i] == ',') {
                if (tc1 == -1) tc1 = i;
                else if (tc2 == -1) tc2 = i;
                else { tc3 = i; break; }
            }
        }
        if (tc1 != -1 && tc2 != -1 && tc3 != -1) {
            try { totalGames = std::stoi(totalStr.substr(0, tc1)); } catch (...) {}
            try { easyCount = std::stoi(totalStr.substr(tc1 + 1, tc2 - tc1 - 1)); } catch (...) {}
            try { mediumCount = std::stoi(totalStr.substr(tc2 + 1, tc3 - tc2 - 1)); } catch (...) {}
            try { hardCount = std::stoi(totalStr.substr(tc3 + 1)); } catch (...) {}
        } else {
            try { totalGames = std::stoi(totalStr); } catch (...) {}
        }
    }

    // Collect session entries by difficulty (lines 1+)
    // Entry format: game_num||mode||difficulty||wpm||accuracy
    std::vector<std::string> easyEntries, mediumEntries, hardEntries;
    for (int i = 1; i < (int)lines.size(); i++) {
        if (lines[i].empty()) continue;
        // Find the difficulty field (3rd field)
        std::string entry = lines[i];
        // Parse fields separated by "||"
        std::vector<std::string> fields;
        int start = 0;
        for (int j = 0; j < (int)entry.size() - 1; j++) {
            if (entry[j] == '|' && entry[j + 1] == '|') {
                fields.push_back(entry.substr(start, j - start));
                start = j + 2;
            }
        }
        fields.push_back(entry.substr(start));

        if (fields.size() >= 5) {
            std::string diff = fields[2];
            if (diff == "Easy") easyEntries.push_back(entry);
            else if (diff == "Medium") mediumEntries.push_back(entry);
            else if (diff == "Hard") hardEntries.push_back(entry);
        }
    }

    auto printDiffStats = [](const std::string& diffName, float avgWpm, int count,
                             const std::vector<std::string>& entries) {
        std::cout << "\n--- " << diffName << " ---\n";
        std::cout << "Games Played: " << count << "\n";
        std::cout << "Average WPM: " << std::fixed << std::setprecision(2) << avgWpm << "\n";
        if (!entries.empty()) {
            std::cout << "Session History:\n";
            for (const auto& e : entries) {
                // Parse entry: game_num||mode||difficulty||wpm||accuracy
                std::vector<std::string> f;
                int s = 0;
                for (int j = 0; j < (int)e.size() - 1; j++) {
                    if (e[j] == '|' && e[j + 1] == '|') {
                        f.push_back(e.substr(s, j - s));
                        s = j + 2;
                    }
                }
                f.push_back(e.substr(s));
                if (f.size() >= 5) {
                    std::cout << "  Game #" << f[0] << " | Mode: " << f[1]
                              << " | WPM: " << f[3] << " | Accuracy: " << f[4] << "%\n";
                }
            }
        }
    };

    std::cout << "\nOverall Best WPM: " << bestWpm << "\n";
    std::cout << "Total Games: " << totalGames << "\n";

    if (statChoice == 1 || statChoice == 4) {
        printDiffStats("Easy", avgEasy, easyCount, easyEntries);
    }
    if (statChoice == 2 || statChoice == 4) {
        printDiffStats("Medium", avgMedium, mediumCount, mediumEntries);
    }
    if (statChoice == 3 || statChoice == 4) {
        printDiffStats("Hard", avgHard, hardCount, hardEntries);
    }

    std::cout << "\n==================\n";
}