#include "TerminalSetup.h"
#include "SpeedTest.h"
#include <iostream>
#include "user_manager.h"
#include "user.h"
#include <string>

int main(){
    std::cout << "=== TYPING SPEED TEST APPLICATION ===\n\n";
    
    int choice;
    std::string name, user_id, password;
    User currentUser;
    bool isLoggedIn = false;
    
    while (true) {
        if (!isLoggedIn) {
            std::cout << "\n=== MAIN MENU ===\n";
            std::cout << "1. Register\n2. Login\n3. Play as Guest\n4. Exit\n";
            std::cout << "Choice: ";
            std::cin >> choice;
            std::cin.ignore(); // Clear newline
            
            switch (choice) {
                case 1: { // Register - Note the brace
                    std::cout << "Enter name: ";
                    std::getline(std::cin, name);
                    std::cout << "Enter user ID: ";
                    std::getline(std::cin, user_id);
                    std::cout << "Enter password: ";
                    std::getline(std::cin, password);
                    
                    if (UserManager::registerUser(name, user_id, password)) {
                        currentUser = UserManager::loadUser(user_id);
                        isLoggedIn = true;
                        std::cout << "\nRegistration successful! Logged in as " << user_id << "\n";
                    }
                    break;
                }
                    
                case 2: { // Login - Note the brace
                    std::cout << "Enter user ID: ";
                    std::getline(std::cin, user_id);
                    std::cout << "Enter password: ";
                    std::getline(std::cin, password);
                    
                    if (UserManager::loginUser(user_id, password)) {
                        currentUser = UserManager::loadUser(user_id);
                        isLoggedIn = true;
                        std::cout << "\nLogin successful! Welcome " << currentUser.getName() << "!\n";
                    }
                    break;
                }
                    
                case 3: { // Play as Guest - Note the brace
                    std::cout << "\nPlaying as Guest...\n";
                    runSpeedTest();
                    std::cout << "\nGuest session completed.\n";
                    break;
                }
                    
                case 4: // Exit
                    std::cout << "\nGoodbye!\n";
                    return 0;
                    
                default:
                    std::cout << "Invalid choice.\n";
            }
        } else {
            // User is logged in - show game menu
            std::cout << "\n=== GAME MENU ===\n";
            std::cout << "Logged in as: " << currentUser.getName() << " (" << currentUser.getUserId() << ")\n";
            std::cout << "1. Start Typing Test\n";
            std::cout << "2. View Stats\n";
            std::cout << "3. Logout\n";
            std::cout << "Choice: ";
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1: { // Start Typing Test - Note the brace
                    std::cout << "\nStarting typing test...\n";
                    
                    // Run test and get results
                    TestResults results = runSpeedTestWithResults();
                    
                    // Update user stats
                    currentUser.addSessionResult(results.wpm, results.accuracy);
                    
                    // Save updated user data
                    UserManager::saveUser(currentUser);
                    
                    // Display session summary
                    currentUser.displaySessionSummary(results.wpm, results.accuracy);
                    
                    // Ask to continue
                    std::cout << "\nPress Enter to return to menu...";
                    std::cin.ignore();
                    break;
                }
                    
                case 2: // View Stats
                    currentUser.displayStats();
                    std::cout << "\nPress Enter to continue...";
                    std::cin.ignore();
                    break;
                    
                case 3: // Logout
                    isLoggedIn = false;
                    std::cout << "\nLogged out successfully.\n";
                    break;
                    
                default:
                    std::cout << "Invalid choice.\n";
            }
        }
    }
    
    return 0;
}