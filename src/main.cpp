#include "TerminalSetup.h"
#include "SpeedTest.h"
#include <iostream>
#include "user_manager.h"
#include "user.h"
#include <string>
#include <file_helper.h>
#include <hashing.h>
#include "session_logger.h"
#include "leaderboard.h"
#include "falling_words.h"
#include "TimeTest.h"
int main(){
    std::cout<<CLEAR_SCREEN;
    std::cout<<"\n";
    std:: cout<< BG_BRIGHT_RED<<BRIGHT_WHITE;
    std::cout << "              === TYPING SPEED TEST APPLICATION ===                                  \n";
    std::cout <<RESET;
    std::cout<<"\n";
    int choice;
    std::string name, user_id, password;
    User currentUser;
    bool isLoggedIn = false;
    
    while (true) {
        if (!isLoggedIn) {
            std::cout<<BRIGHT_MAGENTA;
            std::cout << "\n            === MAIN MENU ===\n";
            std::cout << RESET;
            std::cout << CYAN;
            std::cout << "1. Register\n2. Login\n3. Play as Guest\n4. Exit\n";
            std::cout<<RESET;
            std::cout<<ITALIC<<BRIGHT_YELLOW<<BOLD;
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
                        initCharMap();
                        std::string encryptedUserId = encryptString(padString(user_id, 23));
                        
                        // Load the newly registered user
                        currentUser = UserManager::loadUser(encryptedUserId);
                        currentUser.setUserId(encryptedUserId);
                        
                        isLoggedIn = true;
                        std::cout << "\033[2J\033[H";
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
                        initCharMap();
                        std::string encryptedUserId = encryptString(padString(user_id, 23));
                        
                        // Load the user with encrypted ID and stats
                        currentUser = UserManager::loadUser(encryptedUserId);
                        currentUser.setUserId(encryptedUserId);
                        
                        std::string user_name = FileHandler::getUserNameFromRegistry(encryptedUserId);
                        isLoggedIn = true;
                        std::cout << "\033[2J\033[H";
                        std::cout << "\nLogin successful! Welcome " << decryptString(user_name) << "!\n";
                    }
                    break;
                }
                    
                case 3: { // Play as Guest
                    std::cout << "\n=== GUEST MODE ===\n";
                    std::cout << "Playing as Guest (results won't be saved)...\n";
                    int diff;
                    std::cout << "\nChoose difficulty:\n";
                    std::cout << "1. Easy\n2. Medium\n3. Hard\n";
                    std::cout << "Choice: ";
                    std::cin >> diff;
                    std::cin.ignore();

                    if (diff < 1 || diff > 3) {
                        std::cout << "Invalid choice. Defaulting to Easy.\n";
                        diff = 1;
                    }
                    std::cout << "Press Enter to start...";
                    std::cin.get();
                    
                    // Run test, don't save (saveToUser = false)
                    runSpeedTest(false,diff);
                    
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
            initCharMap();
            std::string encryptedUserId = encryptString(padString(user_id, 23));
            std::string user_name = FileHandler::getUserNameFromRegistry(encryptedUserId);
            std::cout << "Logged in as: " << decryptString(user_name) << " (" << user_id << ")\n";
            std::cout << "1. Start Typing Test\n";
            std::cout << "2. Start Falling Words\n";
            std::cout << "3. Start Time Test\n";
            std::cout << "4. View Stats\n";
            std::cout << "5. View Leaderboard\n";
            std::cout << "6. Logout\n";
            std::cout << "Choice: ";
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1: { // Start Typing Test
                    std::cout << "\nStarting typing test...\n";

                    int diff;
                    std::cout << "\nChoose difficulty:\n";
                    std::cout << "1. Easy\n2. Medium\n3. Hard\n";
                    std::cout << "Choice: ";
                    std::cin >> diff;
                    std::cin.ignore();

                    if (diff < 1 || diff > 3) {
                        std::cout << "Invalid choice. Defaulting to Easy.\n";
                        diff = 1;
                    }
                    std::cout << "Press Enter to begin...";
                    std::cin.get();
                    
                    // Run test with saveToUser = true (but we manually save)
                    TestResults results = runSpeedTest(true,diff);
                    
                    // Update user stats in memory
                    currentUser.addSessionResult(results.wpm, results.accuracy);
                    
                    // Using SessionLogger::logSession instead of UserManager::saveUser() here, as it would overwrite
            
                    std::string modeName = FileHandler::getGameModeString(1);
                    SessionLogger::logSession(encryptedUserId, modeName, diff, results.wpm, results.accuracy);
                    
                    // Submit to leaderboard
                    Leaderboard::submitScore(encryptedUserId, diff, results.wpm, results.accuracy);
                    
                    // Ask to continue
                    std::cout << "\nPress Enter to return to menu...";
                    std::cin.get();
                    break;
                }

                case 2:
                    runFallingWords();
                    std::cout << "\nPress Enter to return to menu...";
                    std::cin.get();
                    break;
                    
                case 3: // Start Time Test
                    runTimeTest();
                    std::cout << "\nPress Enter to return to menu...";
                    std::cin.get();
                    break;
                    
                case 4: // View Stats
                    currentUser.displayStats();
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
                    break;
                    
                case 5: { // View Leaderboard
                    int lbDiff;
                    std::cout << "\nChoose leaderboard:\n";
                    std::cout << "1. Easy\n2. Medium\n3. Hard\n";
                    std::cout << "Choice: ";
                    std::cin >> lbDiff;
                    std::cin.ignore();
                    if (lbDiff < 1 || lbDiff > 3) lbDiff = 1;
                    Leaderboard::displayLeaderboard(lbDiff);
                    std::cout << "\nPress Enter to continue...";
                    std::cin.get();
                    break;
                }
                    
                case 6: // Logout
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