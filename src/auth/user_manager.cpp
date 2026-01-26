#include "user_manager.h"
#include "file_helper.h"
#include <iostream>


const std::string UserManager::USERS_FILE = "../data/users.txt";

bool UserManager::registerUser(const std::string& name, const std::string& user_id, const std::string& password) {
    // ... validation code ...
    
    // Create new instance of a user
    User newUser(name, user_id, password);
    
    // Save user stats to individual file (serialize only stats)
    if (!saveUser(newUser)) {
        std::cout << "Failed to save user data.\n";
        return false;
    }
    
    // Add to registry (users.txt)
    if (!addUserToRegistry(user_id, name, password)) {
        std::cout << "Failed to register user in system.\n";
        return false;
    }
    
    std::cout << "User registered successfully!\n";
    return true;
}

bool UserManager::loginUser(const std::string& user_id, const std::string& password) {
    // Check if user exists
    if (!userExists(user_id)) {
        std::cout << "User ID not found.\n";
        return false;
    }
    
    // Load user data
    User user = loadUser(user_id);
    
    // Check password (plain text for now - add hashing later)
    if (user.getPassword() != password) {
        std::cout << "Incorrect password.\n";
        return false;
    }
    
    std::cout << "Login successful! Welcome " << user.getName() << "!\n";
    //user.displayStats();
    return true;
}

bool UserManager::userExists(const std::string& user_id) {
    return FileHandler::userFileExists(user_id);
}

bool UserManager::saveUser(const User& user) {
    // Only serialize stats (not name, id, password)
    std::string data = user.serialize();
    return FileHandler::saveUserData(user.getUserId(), data);
}

User UserManager::loadUser(const std::string& user_id) {
    User user;
    
    // First, get the user's name and password from the registry
    std::string password = FileHandler::findUserPasswordInRegistry(user_id);
    if (password.empty()) {
        std::cout << "User not found in registry.\n";
        return user; // Return empty user
    }
    
    // Get the name from registry too
    std::string name = FileHandler::getUserNameFromRegistry(user_id);
    
    // Now create the user with basic info
    user = User(name, user_id, password);
    
    // Load stats from individual user file
    std::string data = FileHandler::loadUserData(user_id);
    if (!data.empty()) {
        user.deserialize(data);
    }
    
    return user;
}

bool UserManager::validateUserId(const std::string& user_id) {
    // Check length
    if (user_id.length() < 3 || user_id.length() > 20) {
        return false;
    }
    
    // Check characters (alphanumeric and special characters only)
    // for(char c : user_id){
    //     if ()
    //     return false;
    // }
    
    return true;
}

bool UserManager::validatePassword(const std::string& password) {
    return password.length() >= 6 || password.length() <= 20;
}

bool UserManager::addUserToRegistry(const std::string& user_id, const std::string& name, const std::string& password) {
    if(!FileHandler::directoryExists("data")){
        FileHandler::createDirectory("data");
    }
    std::string line = user_id + ":" + name + ":" + password;
    return FileHandler::writeLine(USERS_FILE, line);
}

