#include "session_logger.h"
#include "file_helper.h"
#include <fstream>
#include <sstream>
#include <iomanip>

std::string SessionLogger::getDifficultyString(int difficulty) {
    if (difficulty == 1) return "Easy";
    if (difficulty == 2) return "Medium";
    if (difficulty == 3) return "Hard";
    return "Easy";
}

std::string SessionLogger::getUserFilePath(const std::string& hashedUserId) {
    return "../data/users/" + hashedUserId + ".txt";
}

SessionLogger::SessionSummary SessionLogger::parseSummaryLine(const std::string& line) {
    SessionSummary summary = {0, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0};

    if (line.empty()) return summary;

    // Format: best_wpm||avg_easy,avg_medium,avg_hard||total_games
    // Find first "||"
    int pos1 = -1;
    for (int i = 0; i < (int)line.size() - 1; i++) {
        if (line[i] == '|' && line[i + 1] == '|') {
            pos1 = i;
            break;
        }
    }
    if (pos1 == -1) return summary;

    // Find second "||"
    int pos2 = -1;
    for (int i = pos1 + 2; i < (int)line.size() - 1; i++) {
        if (line[i] == '|' && line[i + 1] == '|') {
            pos2 = i;
            break;
        }
    }
    if (pos2 == -1) return summary;

    // Parse best_wpm
    std::string bestWpmStr = line.substr(0, pos1);
    try { summary.bestWpm = std::stoi(bestWpmStr); } catch (...) {}

    // Parse avg WPMs: "avg_easy,avg_medium,avg_hard"
    std::string avgStr = line.substr(pos1 + 2, pos2 - pos1 - 2);

    // Find commas
    int comma1 = -1, comma2 = -1;
    for (int i = 0; i < (int)avgStr.size(); i++) {
        if (avgStr[i] == ',') {
            if (comma1 == -1) comma1 = i;
            else { comma2 = i; break; }
        }
    }

    if (comma1 != -1 && comma2 != -1) {
        try { summary.avgWpmEasy = std::stof(avgStr.substr(0, comma1)); } catch (...) {}
        try { summary.avgWpmMedium = std::stof(avgStr.substr(comma1 + 1, comma2 - comma1 - 1)); } catch (...) {}
        try { summary.avgWpmHard = std::stof(avgStr.substr(comma2 + 1)); } catch (...) {}
    }

    // Parse total_games
    std::string totalStr = line.substr(pos2 + 2);

    // Check if totalStr contains difficulty counts: "total_games,easy_count,medium_count,hard_count"
    int tc1 = -1, tc2 = -1, tc3 = -1;
    for (int i = 0; i < (int)totalStr.size(); i++) {
        if (totalStr[i] == ',') {
            if (tc1 == -1) tc1 = i;
            else if (tc2 == -1) tc2 = i;
            else { tc3 = i; break; }
        }
    }

    if (tc1 != -1 && tc2 != -1 && tc3 != -1) {
        try { summary.totalGames = std::stoi(totalStr.substr(0, tc1)); } catch (...) {}
        try { summary.easyCount = std::stoi(totalStr.substr(tc1 + 1, tc2 - tc1 - 1)); } catch (...) {}
        try { summary.mediumCount = std::stoi(totalStr.substr(tc2 + 1, tc3 - tc2 - 1)); } catch (...) {}
        try { summary.hardCount = std::stoi(totalStr.substr(tc3 + 1)); } catch (...) {}
    } else {
        try { summary.totalGames = std::stoi(totalStr); } catch (...) {}
    }

    return summary;
}

std::string SessionLogger::buildSummaryLine(const SessionSummary& summary) {
    std::ostringstream oss;
    oss << summary.bestWpm << "||"
        << std::fixed << std::setprecision(2)
        << summary.avgWpmEasy << ","
        << summary.avgWpmMedium << ","
        << summary.avgWpmHard << "||"
        << summary.totalGames << ","
        << summary.easyCount << ","
        << summary.mediumCount << ","
        << summary.hardCount;
    return oss.str();
}

int SessionLogger::loadSessionCount(const std::string& hashedUserId) {
    std::string filepath = getUserFilePath(hashedUserId);
    std::vector<std::string> lines = FileHandler::readLines(filepath);
    if (lines.empty()) return 0;

    SessionSummary summary = parseSummaryLine(lines[0]);
    return summary.totalGames;
}

bool SessionLogger::logSession(const std::string& hashedUserId,
                                const std::string& modeName,
                                int difficulty,
                                int wpm,
                                int accuracy) {
    std::string filepath = getUserFilePath(hashedUserId);
    std::string diffStr = getDifficultyString(difficulty);

    // Ensure directories exist
    if (!FileHandler::directoryExists("../data")) {
        FileHandler::createDirectory("../data");
    }
    if (!FileHandler::directoryExists("../data/users")) {
        FileHandler::createDirectory("../data/users");
    }

    // Read existing file content
    std::vector<std::string> lines = FileHandler::readLines(filepath);

    // Parse or create summary
    SessionSummary summary = {0, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0};
    if (!lines.empty()) {
        summary = parseSummaryLine(lines[0]);
    }

    // Update best WPM
    if (wpm > summary.bestWpm) {
        summary.bestWpm = wpm;
    }

    // Update per-difficulty average
    if (difficulty == 1) {
        summary.avgWpmEasy = (summary.avgWpmEasy * summary.easyCount + wpm) / (summary.easyCount + 1);
        summary.easyCount++;
    } else if (difficulty == 2) {
        summary.avgWpmMedium = (summary.avgWpmMedium * summary.mediumCount + wpm) / (summary.mediumCount + 1);
        summary.mediumCount++;
    } else {
        summary.avgWpmHard = (summary.avgWpmHard * summary.hardCount + wpm) / (summary.hardCount + 1);
        summary.hardCount++;
    }

    // Increment total games
    summary.totalGames++;

    // Build the new game entry line
    std::ostringstream entryOss;
    entryOss << summary.totalGames << "||"
             << modeName << "||"
             << diffStr << "||"
             << wpm << "||"
             << accuracy;

    // Build the full file content
    std::ostringstream fileContent;
    fileContent << buildSummaryLine(summary) << "\n";

    // Append existing game entries (lines 1+)
    for (int i = 1; i < (int)lines.size(); i++) {
        if (!lines[i].empty()) {
            fileContent << lines[i] << "\n";
        }
    }

    // Append new entry
    fileContent << entryOss.str() << "\n";

    // Write the entire file
    return FileHandler::writeFile(filepath, fileContent.str());
}
