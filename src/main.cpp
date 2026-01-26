#include "TerminalSetup.h"
#include "SpeedTest.h"
#include <iostream>
#include "user_manager.h"
#include "user.h"
#include <string>

int main(){
    std:: cout<< BG_BRIGHT_RED;
    std::cout << "=== TYPING SPEED TEST APPLICATION ===\n\n";
    std::cout <<RESET;
    int choice;
    std::string name, user_id, password;
    User currentUser;
    bool isLoggedIn = false;
    
    while (true) {
        if (!isLoggedIn) {
            std::cout<<BG_BRIGHT_MAGENTA<<BRIGHT_YELLOW;
            std::cout << "\n=== MAIN MENU ===\n";
            std::cout << RESET;
            std::cout << CYAN;
            std::cout << "1. Register\n2. Login\n3. Play as Guest\n4. Exit\n";
            std::cout << "Choice: ";
            std::cout << RESET;
            std::cin >> choice;
            std::cin.ignore(); // Clear newline
            
            switch (choice) {
                case 1: { // Register
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
                    
                case 2: { // Login
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
                    
                case 3: { // Play as Guest
                    std::cout << "\n=== GUEST MODE ===\n";
                    std::cout << "Playing as Guest (results won't be saved)...\n";
                    std::cout << "Press Enter to start...";
                    std::cin.get();
                    
                    // Run test, don't save (saveToUser = false)
                    runSpeedTest(false);
                    
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
                case 1: { // Start Typing Test
                    std::cout << "\nStarting typing test...\n";
                    std::cout << "Press Enter to begin...";
                    std::cin.get();
                    
                    // Run test with saveToUser = true (but we manually save)
                    TestResults results = runSpeedTest(true);
                    
                    // Update user stats with the latest results
                    currentUser.addSessionResult(results.wpm, results.accuracy);
                    
                    // Save updated user data to file
                    UserManager::saveUser(currentUser);
                    
                    // Display session summary
                    std::cout << "\n=== SESSION SUMMARY ===\n";
                    std::cout << "Session WPM: " << results.wpm << "\n";
                    std::cout << "Session Accuracy: " << results.accuracy << "%\n";
                    std::cout << "Best WPM: " << currentUser.getBestWPM() << "\n";
                    std::cout << "Average WPM: " << currentUser.getAvgWPM() << "\n";
                    std::cout << "Total Sessions: " << currentUser.getTotalSessions() << "\n";
                    std::cout << "Results saved!\n";
                    std::cout << "======================\n";
                    
                    // Ask to continue
                    std::cout << "\nPress Enter to return to menu...";
                    std::cin.get();
                    break;
                }
                    
                case 2: // View Stats
                    currentUser.displayStats();
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
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