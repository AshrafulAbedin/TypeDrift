#include "user.h"
#include "file_helper.h"
#include "session_logger.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <hashing.h>
#include "TerminalSetup.h"

User::User() : total_sessions(0), total_wpm(0), best_wpm(0), total_games(0), avg_wpm(0.0f) {}

std::string User::getUserId() const { return user_id; }
int User::getBestWPM() const { return best_wpm; }
int User::getTotalGames() const { return total_games; }
float User::getAvgWPM() const { return avg_wpm; }
void User::setUserId(const std::string& userId){ this->user_id = userId; }

void User::updateStats(int wpm) {
    if (wpm > best_wpm) {
        best_wpm = wpm;
    }
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
    
    if (!std::getline(iss, temp, '|')) return false;
    try { best_wpm = std::stoi(temp); } catch (...) { return false; }
    
    char c;
    iss.get(c);
    if (c != '|') return false;
    
    if (!std::getline(iss, temp, '|')) return false;
    try { avg_wpm = std::stof(temp); } catch (...) { return false; }
    
    iss.get(c);
    if (c != '|') return false;
    
    if (!std::getline(iss, temp)) return false;
    try { total_games = std::stoi(temp); } catch (...) { return false; }
    
    return true;
}

void User::addSessionResult(int wpm, int accuracy) {
    total_games++;
    total_wpm += wpm;
    avg_wpm = (float)total_wpm / total_games;
    
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

// ===================== Helper: parse || delimited fields =====================
static std::vector<std::string> parseFields(const std::string& line) {
    std::vector<std::string> fields;
    int start = 0;
    for (int j = 0; j < (int)line.size() - 1; j++) {
        if (line[j] == '|' && line[j + 1] == '|') {
            fields.push_back(line.substr(start, j - start));
            start = j + 2;
        }
    }
    fields.push_back(line.substr(start));
    return fields;
}

// ===================== Helper: print career difficulty stats =====================
static void printCareerDiffStats(const std::string& diffName, float avgWpm, int count,
                                  const std::vector<std::string>& entries) {
    std::cout << "\n";
    std::cout << "    " << BOLD << CYAN << "--- " << diffName << " ---" << RESET << "\n";
    std::cout << "    " << WHITE << "Games Played: " << BRIGHT_YELLOW << count << RESET << "\n";
    std::cout << "    " << WHITE << "Average WPM:  " << BRIGHT_YELLOW << std::fixed << std::setprecision(2) << avgWpm << RESET << "\n";
    if (!entries.empty()) {
        std::cout << "    " << GRAY << "Session History:" << RESET << "\n";
        for (const auto& e : entries) {
            std::vector<std::string> f = parseFields(e);
            if (f.size() >= 5) {
                std::cout << "      " << GRAY << "Game #" << f[0]
                          << WHITE << " | Mode: " << f[1]
                          << " | WPM: " << BRIGHT_YELLOW << f[3]
                          << WHITE << " | Accuracy: " << GREEN << f[4] << "%" << RESET << "\n";
            }
        }
    }
}

// ===================== displayStats =====================

void User::displayStats() const {
    std::cout << CLEAR_SCREEN;
    std::cout << "\n";
    std::cout << BOLD << CYAN;
    std::cout << "    ╔══════════════════════════════════════╗\n";
    std::cout << "    ║            VIEW  STATS               ║\n";
    std::cout << "    ╚══════════════════════════════════════╝\n";
    std::cout << RESET;

    HASHING_H::initCharMap();
    std::cout << WHITE << "    User: " << BRIGHT_YELLOW << decryptString(user_id) << RESET << "\n\n";

    std::cout << BOLD << MAGNETA << "    -- Choose Category --\n" << RESET;
    std::cout << BRIGHT_YELLOW << "    1. " << RESET << WHITE << "Career Mode\n";
    std::cout << BRIGHT_YELLOW << "    2. " << RESET << WHITE << "Fun Mode\n";
    std::cout << BRIGHT_YELLOW << "    3. " << RESET << WHITE << "All\n";
    std::cout << RESET << "\n";
    std::cout << BOLD << BRIGHT_YELLOW << "    Choice: " << RESET;

    int mainChoice;
    std::cin >> mainChoice;
    std::cin.ignore();

    if (mainChoice < 1 || mainChoice > 3) {
        std::cout << RED << "    Invalid choice.\n" << RESET;
        return;
    }

    bool showCareer = (mainChoice == 1 || mainChoice == 3);
    bool showFun = (mainChoice == 2 || mainChoice == 3);

    int careerSub = 4; // default to "All" for career
    if (mainChoice == 1) {
        std::cout << CLEAR_SCREEN << "\n";
        std::cout << BOLD << CYAN << "    === CAREER MODE STATS ===" << RESET << "\n\n";
        std::cout << BRIGHT_YELLOW << "    1. " << RESET << WHITE << "Easy\n";
        std::cout << BRIGHT_YELLOW << "    2. " << RESET << WHITE << "Medium\n";
        std::cout << BRIGHT_YELLOW << "    3. " << RESET << WHITE << "Hard\n";
        std::cout << BRIGHT_YELLOW << "    4. " << RESET << WHITE << "All\n";
        std::cout << RESET << "\n";
        std::cout << BOLD << BRIGHT_YELLOW << "    Difficulty: " << RESET;
        std::cin >> careerSub;
        std::cin.ignore();
        if (careerSub < 1 || careerSub > 4) careerSub = 4;
    }

    int funSub = 4; // default to "All" for fun
    if (mainChoice == 2) {
        std::cout << CLEAR_SCREEN << "\n";
        std::cout << BOLD << MAGNETA << "    === FUN MODE STATS ===" << RESET << "\n\n";
        std::cout << BRIGHT_YELLOW << "    1. " << RESET << WHITE << "Falling Words\n";
        std::cout << BRIGHT_YELLOW << "    2. " << RESET << WHITE << "Timed Test\n";
        std::cout << BRIGHT_YELLOW << "    3. " << RESET << WHITE << "No Vowel Game\n";
        std::cout << BRIGHT_YELLOW << "    4. " << RESET << WHITE << "All\n";
        std::cout << RESET << "\n";
        std::cout << BOLD << BRIGHT_YELLOW << "    Game: " << RESET;
        std::cin >> funSub;
        std::cin.ignore();
        if (funSub < 1 || funSub > 4) funSub = 4;
    }

    std::cout << CLEAR_SCREEN << "\n";

    // ===== CAREER STATS =====
    if (showCareer) {
        // Try new path first, fall back to legacy
        std::string careerPath = "../data/users/" + user_id + "/career_stats.txt";
        std::vector<std::string> lines = FileHandler::readLines(careerPath);
        if (lines.empty()) {
            // Try legacy path
            std::string legacyPath = "../data/users/" + user_id + ".txt";
            lines = FileHandler::readLines(legacyPath);
        }

        std::cout << BOLD << CYAN;
        std::cout << "    ╔══════════════════════════════════════╗\n";
        std::cout << "    ║          CAREER MODE STATS           ║\n";
        std::cout << "    ╚══════════════════════════════════════╝\n";
        std::cout << RESET;

        if (lines.empty()) {
            std::cout << GRAY << "\n    No career sessions recorded yet.\n" << RESET;
        } else {
            // Parse summary line
            std::string summaryLine = lines[0];
            int bestWpm = 0;
            float avgEasy = 0, avgMedium = 0, avgHard = 0;
            int totalGames = 0, easyCount = 0, mediumCount = 0, hardCount = 0;

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

            // Collect entries by difficulty
            std::vector<std::string> easyEntries, mediumEntries, hardEntries;
            for (int i = 1; i < (int)lines.size(); i++) {
                if (lines[i].empty()) continue;
                std::vector<std::string> fields = parseFields(lines[i]);
                if (fields.size() >= 5) {
                    if (fields[2] == "Easy") easyEntries.push_back(lines[i]);
                    else if (fields[2] == "Medium") mediumEntries.push_back(lines[i]);
                    else if (fields[2] == "Hard") hardEntries.push_back(lines[i]);
                }
            }

            std::cout << "\n    " << WHITE << "Overall Best WPM: " << BRIGHT_YELLOW << BOLD << bestWpm << RESET << "\n";
            std::cout << "    " << WHITE << "Total Career Games: " << BRIGHT_YELLOW << totalGames << RESET << "\n";

            if (careerSub == 1 || careerSub == 4) {
                printCareerDiffStats("Easy", avgEasy, easyCount, easyEntries);
            }
            if (careerSub == 2 || careerSub == 4) {
                printCareerDiffStats("Medium", avgMedium, mediumCount, mediumEntries);
            }
            if (careerSub == 3 || careerSub == 4) {
                printCareerDiffStats("Hard", avgHard, hardCount, hardEntries);
            }
        }

        std::cout << "\n";
    }

    // ===== FUN MODE STATS =====
    if (showFun) {
        std::string funPath = "../data/users/" + user_id + "/funmode_stats.txt";
        std::vector<std::string> funLines = FileHandler::readLines(funPath);

        std::cout << BOLD << MAGNETA;
        std::cout << "    ╔══════════════════════════════════════╗\n";
        std::cout << "    ║           FUN MODE STATS             ║\n";
        std::cout << "    ╚══════════════════════════════════════╝\n";
        std::cout << RESET;

        if (funLines.empty()) {
            std::cout << GRAY << "\n    No fun mode sessions recorded yet.\n" << RESET;
        } else {
            // Parse fun summary line
            std::vector<std::string> summaryFields = parseFields(funLines[0]);
            int bestTTChars = 0, bestNVWpm = 0, bestNVAcc = 0, bestFWScore = 0;
            if (summaryFields.size() >= 5) {
                try { bestTTChars  = std::stoi(summaryFields[1]); } catch (...) {}
                try { bestNVWpm   = std::stoi(summaryFields[2]); } catch (...) {}
                try { bestNVAcc   = std::stoi(summaryFields[3]); } catch (...) {}
                try { bestFWScore = std::stoi(summaryFields[4]); } catch (...) {}
            }

            // Collect fun mode entries by game type
            std::vector<std::string> fallingEntries, timeEntries, vowelEntries;
            for (int i = 1; i < (int)funLines.size(); i++) {
                if (funLines[i].empty()) continue;
                std::vector<std::string> f = parseFields(funLines[i]);
                if (f.size() >= 3) {
                    if (f[1] == "FallingWords") fallingEntries.push_back(funLines[i]);
                    else if (f[1] == "TimeTest") timeEntries.push_back(funLines[i]);
                    else if (f[1] == "VowelGame") vowelEntries.push_back(funLines[i]);
                }
            }

            // Falling Words
            if (funSub == 1 || funSub == 4) {
                std::cout << "\n    " << BOLD << BRIGHT_CYAN << "--- Falling Words ---" << RESET << "\n";
                std::cout << "    " << WHITE << "Best Score: " << BRIGHT_YELLOW << bestFWScore << RESET << "\n";
                std::cout << "    " << WHITE << "Games Played: " << BRIGHT_YELLOW << fallingEntries.size() << RESET << "\n";
                if (!fallingEntries.empty()) {
                    std::cout << "    " << GRAY << "History:" << RESET << "\n";
                    for (const auto& e : fallingEntries) {
                        std::vector<std::string> f = parseFields(e);
                        if (f.size() >= 3) {
                            std::cout << "      " << GRAY << "Score: " << BRIGHT_YELLOW << f[2] << RESET << "\n";
                        }
                    }
                }
            }

            // Timed Test
            if (funSub == 2 || funSub == 4) {
                std::cout << "\n    " << BOLD << BRIGHT_CYAN << "--- Timed Test ---" << RESET << "\n";
                std::cout << "    " << WHITE << "Best Characters: " << BRIGHT_YELLOW << bestTTChars << RESET << "\n";
                std::cout << "    " << WHITE << "Games Played: " << BRIGHT_YELLOW << timeEntries.size() << RESET << "\n";
                if (!timeEntries.empty()) {
                    std::cout << "    " << GRAY << "History:" << RESET << "\n";
                    for (const auto& e : timeEntries) {
                        std::vector<std::string> f = parseFields(e);
                        if (f.size() >= 3) {
                            std::cout << "      " << GRAY << "Characters: " << BRIGHT_YELLOW << f[2] << RESET << "\n";
                        }
                    }
                }
            }

            // No Vowel Game
            if (funSub == 3 || funSub == 4) {
                std::cout << "\n    " << BOLD << BRIGHT_CYAN << "--- No Vowel Game ---" << RESET << "\n";
                std::cout << "    " << WHITE << "Best WPM: " << BRIGHT_YELLOW << bestNVWpm << RESET << "\n";
                std::cout << "    " << WHITE << "Best Accuracy: " << GREEN << bestNVAcc << "%" << RESET << "\n";
                std::cout << "    " << WHITE << "Games Played: " << BRIGHT_YELLOW << vowelEntries.size() << RESET << "\n";
                if (!vowelEntries.empty()) {
                    std::cout << "    " << GRAY << "History:" << RESET << "\n";
                    for (const auto& e : vowelEntries) {
                        std::vector<std::string> f = parseFields(e);
                        if (f.size() >= 4) {
                            std::cout << "      " << GRAY << "WPM: " << BRIGHT_YELLOW << f[2]
                                      << WHITE << " | Accuracy: " << GREEN << f[3] << "%" << RESET << "\n";
                        }
                    }
                }
            }
        }

        std::cout << "\n";
    }

    std::cout << "    " << GRAY;
    for (int i = 0; i < 42; i++) std::cout << "═";
    std::cout << RESET << "\n";
}