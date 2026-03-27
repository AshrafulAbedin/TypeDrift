#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>

class FileHandler {
public:
    // Call once from main() with argv[0] to anchor all data paths
    static void setDataDir(const std::string& executablePath);
    // Returns an absolute path to a file/dir inside the data directory
    static std::string getDataPath(const std::string& relative);

    // File operations
    static bool fileExists(const std::string& filename);
    static bool createFile(const std::string& filename);
    static bool deleteFile(const std::string& filename);
    
    // Reading operations
    static std::string readFile(const std::string& filename);
    static std::vector<std::string> readLines(const std::string& filename);
    
    // Writing operations
    static bool writeFile(const std::string& filename, const std::string& content);
    static bool appendToFile(const std::string& filename, const std::string& content);
    static bool writeLine(const std::string& filename, const std::string& line);
    
    // Directory operations
    static bool directoryExists(const std::string& path);
    static bool createDirectory(const std::string& path);
    
    // User data operations
    static bool saveUserData(const std::string& user_id, const std::string& data);
    static std::string loadUserData(const std::string& user_id);
    static bool userFileExists(const std::string& user_id);
    static std::string findUserPasswordInRegistry(const std::string& user_id); 
    static std::string getUserNameFromRegistry(const std::string& user_id);

    // Game info helpers
    static std::string getDifficultyString(int difficulty);
    static std::string getGameModeString(int mode);

private:
    static std::string s_dataDir; // absolute path to the data/ directory
};

#endif // FILE_HANDLER_H