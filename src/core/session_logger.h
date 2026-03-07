#ifndef SESSION_LOGGER_H
#define SESSION_LOGGER_H

#include <string>
#include <vector>

class SessionLogger {
public:
    // Log a career mode session (SpeedTest easy/medium/hard)
    // Writes to data/users/<hashedUserId>/career_stats.txt
    static bool logSession(const std::string& hashedUserId,
                           const std::string& modeName,
                           int difficulty,
                           int wpm,
                           int accuracy);

    // Log a fun mode session
    // Writes to data/users/<hashedUserId>/funmode_stats.txt
    static bool logFunSession(const std::string& hashedUserId,
                              const std::string& gameName,
                              int wpm,
                              int accuracy,
                              int score = -1);

    // Convert difficulty int to string
    static std::string getDifficultyString(int difficulty);

    // Get total game count from user file
    static int loadSessionCount(const std::string& hashedUserId);

    // Ensure user directory exists
    static void ensureUserDir(const std::string& hashedUserId);

    // Migration: move old userid.txt data to userid/career_stats.txt
    static void migrateOldUserFile(const std::string& hashedUserId);

private:
    // Parse the summary line (line 1) from career stats file
    struct SessionSummary {
        int bestWpm;
        float avgWpmEasy;
        float avgWpmMedium;
        float avgWpmHard;
        int totalGames;
        int easyCount;
        int mediumCount;
        int hardCount;
    };

    // Fun mode summary (line 1 of funmode_stats.txt)
    struct FunModeSummary {
        int bestTimetestChars;   // best characters typed in a TimeTest session
        int bestNovowelWpm;
        int bestNovowelAccuracy;
        int bestFallingScore;
    };

    static SessionSummary parseSummaryLine(const std::string& line);
    static std::string buildSummaryLine(const SessionSummary& summary);

    static FunModeSummary parseFunSummaryLine(const std::string& line);
    static std::string buildFunSummaryLine(const FunModeSummary& summary);

    static std::string getCareerFilePath(const std::string& hashedUserId);
    static std::string getFunModeFilePath(const std::string& hashedUserId);

    // Legacy path (pre-migration)
    static std::string getLegacyFilePath(const std::string& hashedUserId);
};

#endif // SESSION_LOGGER_H
