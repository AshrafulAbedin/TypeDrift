#ifndef USER_H
#define USER_H

#include <string>

class User {
 private:
    std::string name;
    std::string user_id;
//     std::string password;
    int total_sessions;  
    int total_wpm;
    int best_wpm;
    int total_games;
    float avg_wpm;
    
public:
    // Constructors
    User();
    //User(const std::string& name, const std::string& user_id, const std::string& password);
    
    // Getters
 //    std::string getName() const;
     std::string getUserId() const;
    // std::string getPassword() const;
    int getBestWPM() const;
    int getTotalGames() const;
    float getAvgWPM() const;
    int getTotalSessions() const;
    int getAverageWPM() const;
    
    // Setters
    // void setName(const std::string& name);
    // void setPassword(const std::string& password);
    void setUserId(const std::string& userId);
    void updateStats(int wpm);
    
    // Serialization
    std::string serialize() const;
    void addSessionResult(int wpm, int accuracy);
    bool deserialize(const std::string& data);
    
    // Stats display
    void displayStats() const;
    void displaySessionSummary(int wpm, int accuracy) const;
    
};

#endif // USER_H