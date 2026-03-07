#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <vector>

class Leaderboard {
public:
    static const int MAX_ENTRIES = 100;

    struct LeaderboardEntry {
        std::string userId;
        int wpm;
        int accuracy;
        float score;  // composite score = wpm * accuracy / 100.0 (career), or raw score (falling)
    };

    // Career mode: submit score for easy/medium/hard
    static bool submitScore(const std::string& userId, int difficulty, int wpm, int accuracy);

    // Fun mode: submit score for a fun game
    static bool submitFunScore(const std::string& userId, const std::string& gameName,
                               int wpm, int accuracy, int rawScore = -1);

    // Career mode: load & display leaderboard
    static std::vector<LeaderboardEntry> loadLeaderboard(int difficulty);
    static void displayLeaderboard(int difficulty);

    // Fun mode: load & display leaderboard
    static std::vector<LeaderboardEntry> loadFunLeaderboard(const std::string& gameName);
    static void displayFunLeaderboard(const std::string& gameName);

private:
    // Career mode paths
    static std::string getLeaderboardFilePath(int difficulty);
    static std::string getDifficultyString(int difficulty);

    // Fun mode paths
    static std::string getFunLeaderboardFilePath(const std::string& gameName);
    static std::string getFunGameDisplayName(const std::string& gameName);

    // Shared helpers
    static std::vector<LeaderboardEntry> parseLeaderboardFile(const std::string& filepath);
    static bool writeLeaderboardFile(const std::string& filepath, const std::vector<LeaderboardEntry>& entries);
};

#endif // LEADERBOARD_H
