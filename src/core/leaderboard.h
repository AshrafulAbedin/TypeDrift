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
        float score;  // composite score = wpm * accuracy / 100.0
    };

    // Submit a score after a game — inserts into the correct difficulty leaderboard if it qualifies
    static bool submitScore(const std::string& userId, int difficulty, int wpm, int accuracy);

    // Load all entries from a difficulty leaderboard
    static std::vector<LeaderboardEntry> loadLeaderboard(int difficulty);

    // Display a difficulty leaderboard to the terminal
    static void displayLeaderboard(int difficulty);

private:
    static std::string getLeaderboardFilePath(int difficulty);
    static std::string getDifficultyString(int difficulty);
    static std::vector<LeaderboardEntry> parseLeaderboardFile(const std::string& filepath);
    static bool writeLeaderboardFile(const std::string& filepath, const std::vector<LeaderboardEntry>& entries);
};

#endif // LEADERBOARD_H
