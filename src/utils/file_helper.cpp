#include "file_helper.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h> // For directory operations
#include <filesystem>

#ifdef _WIN32 //For Windows
    #include <direct.h>
    #define MKDIR(a) _mkdir(a)
#else //For Unix like system
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(a) mkdir(a, 0755)
#endif

// Static member definition
std::string FileHandler::s_dataDir = "../data"; // fallback if setDataDir not called

void FileHandler::setDataDir(const std::string& executablePath) {
    // Resolve the executable's directory, then go up one level to find data/
    try {
        std::filesystem::path exePath = std::filesystem::canonical(executablePath);
        // data/ is a sibling of the build/ dir, i.e. one level up from the exe
        std::filesystem::path dataDir = exePath.parent_path().parent_path() / "data";
        s_dataDir = dataDir.string();
    } catch (...) {
        // fallback: keep default
        s_dataDir = "../data";
    }
}

std::string FileHandler::getDataPath(const std::string& relative) {
    if (relative.empty()) return s_dataDir;
    // Avoid double-slash
    if (relative[0] == '/' || relative[0] == '\\') {
        return s_dataDir + relative;
    }
    return s_dataDir + "/" + relative;
}

bool FileHandler::fileExists(const std::string& filename) {
    std::ifstream file(filename); // ifstream input file stream tries to read to file
    return file.good();
}

bool FileHandler::createFile(const std::string& filename) {
    std::ofstream file(filename); //ofstream output file stream tries to write to file
    return file.is_open();
}

bool FileHandler::deleteFile(const std::string& filename) {
    return std::remove(filename.c_str()) == 0; //c_str converts string file name to char * array because remove expects a char *
}

std::string FileHandler::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),//start pointer
                         std::istreambuf_iterator<char>());//End pointer
    return content;
}

std::vector<std::string> FileHandler::readLines(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return lines;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

bool FileHandler::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

bool FileHandler::appendToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

bool FileHandler::writeLine(const std::string& filename, const std::string& line) {
    return appendToFile(filename, line + "\n");
}

bool FileHandler::directoryExists(const std::string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

bool FileHandler::createDirectory(const std::string& path) {
    return MKDIR(path.c_str()) == 0;
}

// User-specific operations
bool FileHandler::saveUserData(const std::string& user_id, const std::string& data) {
    std::string data_dir = getDataPath("");
    std::string user_dir = getDataPath("users");
    std::string user_subdir = getDataPath("users/" + user_id);

    if (!directoryExists(data_dir)) {
        if (!createDirectory(data_dir)) {
            std::cerr << "Failed to create data directory: " << data_dir << "\n";
            return false;
        }
    }
    if (!directoryExists(user_dir)) {
        if (!createDirectory(user_dir)) {
            std::cerr << "Failed to create users directory: " << user_dir << "\n";
            return false;
        }
    }
    // Only create the user's sub-directory; no flat .txt file is written.
    if (!directoryExists(user_subdir)) {
        if (!createDirectory(user_subdir)) {
            std::cerr << "Failed to create user directory: " << user_subdir << "\n";
            return false;
        }
    }
    return true;
}

std::string FileHandler::loadUserData(const std::string& user_id) {
    // Stats are stored in career_stats.txt and funmode_stats.txt inside the user directory.
    // This function is kept for compatibility but returns empty; callers use session_logger directly.
    return "";
}

bool FileHandler::userFileExists(const std::string& user_id) {
    // A user exists if their sub-directory exists under data/users/
    std::string user_subdir = getDataPath("users/" + user_id);
    return directoryExists(user_subdir);
}

std::string FileHandler::findUserPasswordInRegistry(const std::string& user_id) {
    auto lines = FileHandler::readLines(getDataPath("users.txt"));
    
    const int FIELD_LENGTH = 23;
    const int EXPECTED_LINE_LENGTH = FIELD_LENGTH * 3 + 2; // 3 fields + 2 spaces
    
    for (const auto& line : lines) {
        if (line.length() < EXPECTED_LINE_LENGTH) continue;
        
        // Extract fields by fixed positions
        std::string stored_id = line.substr(0, FIELD_LENGTH);
        
        if (stored_id == user_id) {
            // Password starts at position: FIELD_LENGTH + 1 (space) + FIELD_LENGTH + 1 (space)
            std::string password = line.substr(FIELD_LENGTH * 2 + 2, FIELD_LENGTH);
            return password;
        }
    }
    
    return "";
}

std::string FileHandler::getUserNameFromRegistry(const std::string& user_id) {
    auto lines = FileHandler::readLines(getDataPath("users.txt"));
    
    const int FIELD_LENGTH = 23;
    const int EXPECTED_LINE_LENGTH = FIELD_LENGTH * 3 + 2; // 3 fields + 2 spaces
    
    for (const auto& line : lines) {
        if (line.length() < EXPECTED_LINE_LENGTH) continue;
        
        // Extract fields by fixed positions
        std::string stored_id = line.substr(0, FIELD_LENGTH);
        
        if (stored_id == user_id) {
            // Name starts at position: FIELD_LENGTH + 1 (space)
            std::string name = line.substr(FIELD_LENGTH + 1, FIELD_LENGTH);
            return name;
        }
    }
    
    return "";
}

std::string FileHandler::getDifficultyString(int difficulty) {
    if (difficulty == 1) return "Easy";
    if (difficulty == 2) return "Medium";
    if (difficulty == 3) return "Hard";
    return "Easy";
}

std::string FileHandler::getGameModeString(int mode) {
    if (mode == 1) return "SpeedTest";
    // Add more modes here as they are created
    return "SpeedTest";
}