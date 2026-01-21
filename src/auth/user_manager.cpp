#include "user_manager.h"
#include "file_helper.h"
#include <iostream>


const std::string UserManager::USERS_FILE = "../data/users.txt";

bool UserManager::registerUser(const std::string& name, const std::string& user_id, const std::string& password) {
    // Check if user already exists
    if (userExists(user_id)) {
        std::cout << "User ID already exists. Please choose another.\n";
        return false;
    }
    
    // Validate inputs
    if (!validateUserId(user_id)) {
        std::cout << "Invalid user ID. Must be 3-20 characters, letters, numbers, and underscores only.\n";
        return false;
    }
    
    if (!validatePassword(password)) {
        std::cout << "Invalid password. Must be at least 6 characters.\n";
        return false;
    }
    
    // Create new instance of a user
    User newUser(name, user_id, password);
    
    // Save user data to individual file
    if (!saveUser(newUser)) {
        std::cout << "Failed to save user data.\n"; //Catch error
        return false;
    }
    
    // Add to registry
    if (!addUserToRegistry(user_id, name)) {
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
    std::string data = user.serialize();
    return FileHandler::saveUserData(user.getUserId(), data);
}

User UserManager::loadUser(const std::string& user_id) {
    User user;
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
    
    // Check characters (alphanumeric and underscore only)
    for(char c : user_id){
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) && c != '_')
        return false;
    }
    
    return true;
}

bool UserManager::validatePassword(const std::string& password) {
    return password.length() >= 6;
}

bool UserManager::addUserToRegistry(const std::string& user_id, const std::string& name) {
    if(!FileHandler::directoryExists("data")){
        FileHandler::createDirectory("data");
    }
    std::string line = user_id + ":" + name;
    return FileHandler::writeLine(USERS_FILE, line);
}

bool UserManager::findUserInRegistry(const std::string& user_id, std::string& name) {
    auto lines = FileHandler::readLines(USERS_FILE);
    
    for (const auto& line : lines) { //taking line by line
        size_t pos = line.find(':'); //finding : so we can take the user_id, pos stores the position of :
        if (pos != std::string::npos) { //if there is user_id and file is not empty(npos)
            std::string stored_id = line.substr(0, pos); //take the sub-string upto pos
            if (stored_id == user_id) { //user found
                name = line.substr(pos + 1); //update the name
                return true;
            }
        }
    }
    
    return false;
}