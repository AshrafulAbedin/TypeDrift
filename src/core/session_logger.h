#ifndef SESSION_LOGGER_H
#define SESSION_LOGGER_H

#include <string>
#include <vector>

class SessionLogger {
public:
    // Log a game session for a logged-in user
    // Writes to data/users/<hashedUserId>.txt
    static bool logSession(const std::string& hashedUserId,
                           const std::string& modeName,
                           int difficulty,
                           int wpm,
                           int accuracy);

    // Convert difficulty int to string
    static std::string getDifficultyString(int difficulty);

    // Get total game count from user file
    static int loadSessionCount(const std::string& hashedUserId);

private:
    // Parse the summary line (line 1) from a user file
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

    static SessionSummary parseSummaryLine(const std::string& line);
    static std::string buildSummaryLine(const SessionSummary& summary);
    static std::string getUserFilePath(const std::string& hashedUserId);
};

#endif // SESSION_LOGGER_H
