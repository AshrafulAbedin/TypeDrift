#include "user.h"
#include "file_helper.h"
#include <sstream>
#include <iomanip> //For setPreacision in serialization & display
#include <iostream>

User::User() : total_sessions(0), total_wpm(0), best_wpm(0), total_games(0), avg_wpm(0.0f) {}

User::User(const std::string& name, const std::string& user_id, const std::string& password)
    : name(name), user_id(user_id), password(password), total_sessions(0), total_wpm(0), best_wpm(0), total_games(0), avg_wpm(0.0f) {}

std::string User::getName() const { return name; }
std::string User::getUserId() const { return user_id; }
std::string User::getPassword() const { return password; }
int User::getBestWPM() const { return best_wpm; }
int User::getTotalGames() const { return total_games; }
float User::getAvgWPM() const { return avg_wpm; }


void User::setName(const std::string& name) { this->name = name; }
void User::setPassword(const std::string& password) { this->password = password; }

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
        << total_sessions;
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
    total_sessions++;
    total_wpm += wpm;
    avg_wpm = (float)total_wpm / total_sessions;
    
    // Update best WPM if needed
    if (wpm > best_wpm) {
        best_wpm = wpm;
    }
    std::ostringstream oss;
    oss << best_wpm << "||" << std::fixed << std::setprecision(2) << avg_wpm << "||" << total_sessions;
    std::string data = oss.str();
    FileHandler::saveUserData(user_id, data);
}

void User::displaySessionSummary(int wpm, int accuracy) const {
    std::cout << "\n=== SESSION SUMMARY ===\n";
    std::cout << "Current Session WPM: " << wpm << "\n";
    std::cout << "Current Session Accuracy: " << accuracy << "%\n";
    std::cout << "Best WPM: " << best_wpm << "\n";
    std::cout << "Average WPM: " << avg_wpm << "\n";
    std::cout << "Total Sessions: " << total_sessions << "\n";
    std::cout << "=====================\n";
}

int User::getTotalSessions() const {
    return total_sessions;
}

int User::getAverageWPM() const {
    return avg_wpm;
}

void User::displayStats() const {
    std::cout << "\n=== User Stats ===\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "User ID: " << user_id << "\n";
    std::cout << "Best WPM: " << best_wpm << "\n";
    std::cout << "Total Games: " << total_games << "\n";
    std::cout << "Average WPM: " << std::fixed << std::setprecision(2) << avg_wpm << "\n";
    std::cout << "==================\n";
}