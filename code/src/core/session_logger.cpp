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

std::string SessionLogger::getLegacyFilePath(const std::string& hashedUserId) {
    return FileHandler::getDataPath("users/" + hashedUserId + ".txt");
}

std::string SessionLogger::getCareerFilePath(const std::string& hashedUserId) {
    return FileHandler::getDataPath("users/" + hashedUserId + "/career_stats.txt");
}

std::string SessionLogger::getFunModeFilePath(const std::string& hashedUserId) {
    return FileHandler::getDataPath("users/" + hashedUserId + "/funmode_stats.txt");
}

void SessionLogger::ensureUserDir(const std::string& hashedUserId) {
    if (!FileHandler::directoryExists(FileHandler::getDataPath(""))) {
        FileHandler::createDirectory(FileHandler::getDataPath(""));
    }
    if (!FileHandler::directoryExists(FileHandler::getDataPath("users"))) {
        FileHandler::createDirectory(FileHandler::getDataPath("users"));
    }
    std::string userDir = FileHandler::getDataPath("users/" + hashedUserId);
    if (!FileHandler::directoryExists(userDir)) {
        FileHandler::createDirectory(userDir);
    }
}

void SessionLogger::migrateOldUserFile(const std::string& hashedUserId) {
    std::string legacyPath = getLegacyFilePath(hashedUserId);
    std::string careerPath = getCareerFilePath(hashedUserId);

    // Only migrate if old file exists and new file doesn't
    if (FileHandler::fileExists(legacyPath) && !FileHandler::fileExists(careerPath)) {
        ensureUserDir(hashedUserId);
        std::string content = FileHandler::readFile(legacyPath);
        if (!content.empty()) {
            FileHandler::writeFile(careerPath, content);
        }
    }
}

// ===================== Career mode summary parsing =====================

SessionLogger::SessionSummary SessionLogger::parseSummaryLine(const std::string& line) {
    SessionSummary summary = {0, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0};

    if (line.empty()) return summary;

    // Format: best_wpm||avg_easy,avg_medium,avg_hard||total_games,easy_count,medium_count,hard_count
    int pos1 = -1;
    for (int i = 0; i < (int)line.size() - 1; i++) {
        if (line[i] == '|' && line[i + 1] == '|') {
            pos1 = i;
            break;
        }
    }
    if (pos1 == -1) return summary;

    int pos2 = -1;
    for (int i = pos1 + 2; i < (int)line.size() - 1; i++) {
        if (line[i] == '|' && line[i + 1] == '|') {
            pos2 = i;
            break;
        }
    }
    if (pos2 == -1) return summary;

    std::string bestWpmStr = line.substr(0, pos1);
    try { summary.bestWpm = std::stoi(bestWpmStr); } catch (...) {}

    std::string avgStr = line.substr(pos1 + 2, pos2 - pos1 - 2);
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

    std::string totalStr = line.substr(pos2 + 2);
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

// ===================== Fun mode summary parsing =====================

SessionLogger::FunModeSummary SessionLogger::parseFunSummaryLine(const std::string& line) {
    FunModeSummary summary = {0, 0, 0, 0};
    if (line.empty()) return summary;

    // Format: Funmode||TimetestChars||NovowelWpm||NovowelAccuracy||FallingBestScore
    std::vector<std::string> fields;
    int start = 0;
    for (int j = 0; j < (int)line.size() - 1; j++) {
        if (line[j] == '|' && line[j + 1] == '|') {
            fields.push_back(line.substr(start, j - start));
            start = j + 2;
        }
    }
    fields.push_back(line.substr(start));

    // fields[0] = "Funmode", fields[1..4] = values
    if (fields.size() >= 5) {
        try { summary.bestTimetestChars = std::stoi(fields[1]); } catch (...) {}
        try { summary.bestNovowelWpm = std::stoi(fields[2]); } catch (...) {}
        try { summary.bestNovowelAccuracy = std::stoi(fields[3]); } catch (...) {}
        try { summary.bestFallingScore = std::stoi(fields[4]); } catch (...) {}
    }

    return summary;
}

std::string SessionLogger::buildFunSummaryLine(const FunModeSummary& summary) {
    std::ostringstream oss;
    oss << "Funmode||"
        << summary.bestTimetestChars << "||"
        << summary.bestNovowelWpm << "||"
        << summary.bestNovowelAccuracy << "||"
        << summary.bestFallingScore;
    return oss.str();
}

// ===================== Session count =====================

int SessionLogger::loadSessionCount(const std::string& hashedUserId) {
    std::string filepath = getCareerFilePath(hashedUserId);
    std::vector<std::string> lines = FileHandler::readLines(filepath);
    if (lines.empty()) {
        // Try legacy
        filepath = getLegacyFilePath(hashedUserId);
        lines = FileHandler::readLines(filepath);
    }
    if (lines.empty()) return 0;

    SessionSummary summary = parseSummaryLine(lines[0]);
    return summary.totalGames;
}

// ===================== Career mode logging =====================

bool SessionLogger::logSession(const std::string& hashedUserId,
                                const std::string& modeName,
                                int difficulty,
                                int wpm,
                                int accuracy) {
    // Ensure user directory exists
    ensureUserDir(hashedUserId);

    // Migrate old file if needed
    migrateOldUserFile(hashedUserId);

    std::string filepath = getCareerFilePath(hashedUserId);
    std::string diffStr = getDifficultyString(difficulty);

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

// ===================== Fun mode logging =====================

bool SessionLogger::logFunSession(const std::string& hashedUserId,
                                   const std::string& gameName,
                                   int wpm,
                                   int accuracy,
                                   int score) {
    // Ensure user directory exists
    ensureUserDir(hashedUserId);

    std::string filepath = getFunModeFilePath(hashedUserId);

    // Read existing file content
    std::vector<std::string> lines = FileHandler::readLines(filepath);

    // Parse or create fun mode summary
    FunModeSummary funSummary = {0, 0, 0, 0};
    if (!lines.empty()) {
        funSummary = parseFunSummaryLine(lines[0]);
    }

    // Update best scores based on game type
    if (gameName == "TimeTest") {
        if (score > funSummary.bestTimetestChars) {
            funSummary.bestTimetestChars = score;
        }
    } else if (gameName == "VowelGame") {
        if (wpm > funSummary.bestNovowelWpm) {
            funSummary.bestNovowelWpm = wpm;
        }
        if (accuracy > funSummary.bestNovowelAccuracy) {
            funSummary.bestNovowelAccuracy = accuracy;
        }
    } else if (gameName == "FallingWords") {
        if (score > funSummary.bestFallingScore) {
            funSummary.bestFallingScore = score;
        }
    }

    // Build the new entry line
    std::ostringstream entryOss;
    if (gameName == "FallingWords") {
        entryOss << "Fun mode||" << gameName << "||" << score;
    } else if (gameName == "TimeTest") {
        entryOss << "Fun mode||" << gameName << "||" << score << " chars";
    } else {
        entryOss << "Fun mode||" << gameName << "||" << wpm << "||" << accuracy;
    }

    // Build the full file content
    std::ostringstream fileContent;
    fileContent << buildFunSummaryLine(funSummary) << "\n";

    // Append existing game entries (lines 1+)
    for (int i = 1; i < (int)lines.size(); i++) {
        if (!lines[i].empty()) {
            fileContent << lines[i] << "\n";
        }
    }

    // Append new entry
    fileContent << entryOss.str() << "\n";

    return FileHandler::writeFile(filepath, fileContent.str());
}
