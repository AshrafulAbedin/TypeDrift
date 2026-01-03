#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <string>
#include "../utils/user.h"

class UserManager {
private:
    static const std::string USERS_FILE;
    
public:
    // User operations
    static bool registerUser(const std::string& name, const std::string& user_id, const std::string& password);
    static bool loginUser(const std::string& user_id, const std::string& password);
    static bool userExists(const std::string& user_id);
    static bool saveUser(const User& user);
    static User loadUser(const std::string& user_id);
    
    // Helper functions
    static bool validateUserId(const std::string& user_id);
    static bool validatePassword(const std::string& password);
    
private:
    static bool addUserToRegistry(const std::string& user_id, const std::string& name);
    static bool findUserInRegistry(const std::string& user_id, std::string& name);
};

#endif // USER_MANAGER_H