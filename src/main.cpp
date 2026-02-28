#include "TerminalSetup.h"
#include "SpeedTest.h"
#include "TimeTest.h"
#include "falling_words.h"
#include "vowel_game.h"
#include <iostream>
#include "user_manager.h"
#include "user.h"
#include <string>
#include <file_helper.h>
#include <hashing.h>
#include "session_logger.h"
#include "leaderboard.h"

int main(){
    std::cout << CLEAR_SCREEN;

    int choice;
    std::string name, user_id, password;
    User currentUser;
    bool isLoggedIn = false;

    while (true) {
        if (!isLoggedIn) {
            // ============ MAIN MENU (not logged in) ============
            std::cout << CLEAR_SCREEN;
            std::cout << "\n";
            std::cout << BOLD << CYAN;
            std::cout << "    ╔══════════════════════════════════════╗\n";
            std::cout << "    ║         T Y P E D R I F T            ║\n";
            std::cout << "    ║     Command-Line Typing Trainer      ║\n";
            std::cout << "    ╚══════════════════════════════════════╝\n";
            std::cout << RESET << "\n";

            std::cout << GRAY << ITALIC;
            std::cout << "    Sharpen your typing skills. Track your progress.\n";
            std::cout << RESET << "\n";

            std::cout << BRIGHT_YELLOW << "    1. " << RESET << WHITE << "Register\n";
            std::cout << BRIGHT_YELLOW << "    2. " << RESET << WHITE << "Login\n";
            std::cout << BRIGHT_YELLOW << "    3. " << RESET << WHITE << "Play as Guest\n";
            std::cout << BRIGHT_YELLOW << "    4. " << RESET << WHITE << "Exit\n";
            std::cout << RESET << "\n";

            std::cout << BOLD << BRIGHT_YELLOW << "    Choice: " << RESET;
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
                case 1: { // Register
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n";
                    std::cout << BOLD << CYAN << "    === REGISTER ===" << RESET << "\n\n";

                    std::cout << WHITE << "    Enter name: " << RESET;
                    std::getline(std::cin, name);
                    std::cout << WHITE << "    Enter user ID: " << RESET;
                    std::getline(std::cin, user_id);
                    std::cout << WHITE << "    Enter password: " << RESET;
                    std::getline(std::cin, password);

                    if (UserManager::registerUser(name, user_id, password)) {
                        initCharMap();
                        std::string encryptedUserId = encryptString(padString(user_id, 23));

                        currentUser = UserManager::loadUser(encryptedUserId);
                        currentUser.setUserId(encryptedUserId);

                        isLoggedIn = true;
                        std::cout << CLEAR_SCREEN;
                        std::cout << "\n";
                        std::cout << GREEN << BOLD << "    Registration successful! " << RESET;
                        std::cout << WHITE << "Logged in as " << BRIGHT_YELLOW << user_id << RESET << "\n";
                    }
                    break;
                }

                case 2: { // Login
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n";
                    std::cout << BOLD << CYAN << "    === LOGIN ===" << RESET << "\n\n";

                    std::cout << WHITE << "    Enter user ID: " << RESET;
                    std::getline(std::cin, user_id);
                    std::cout << WHITE << "    Enter password: " << RESET;
                    std::getline(std::cin, password);

                    if (UserManager::loginUser(user_id, password)) {
                        initCharMap();
                        std::string encryptedUserId = encryptString(padString(user_id, 23));

                        currentUser = UserManager::loadUser(encryptedUserId);
                        currentUser.setUserId(encryptedUserId);

                        std::string user_name = FileHandler::getUserNameFromRegistry(encryptedUserId);
                        isLoggedIn = true;
                        std::cout << CLEAR_SCREEN;
                        std::cout << "\n";
                        std::cout << GREEN << BOLD << "    Login successful! " << RESET;
                        std::cout << WHITE << "Welcome " << BRIGHT_YELLOW << decryptString(user_name) << RESET << "\n";
                    } else {
                        std::cout << RED << "\n    Invalid user ID or password.\n" << RESET;
                        std::cout << GRAY << "    Press Enter to continue..." << RESET;
                        std::cin.get();
                    }
                    break;
                }

                case 3: { // Play as Guest
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n";
                    std::cout << BOLD << CYAN << "    === GUEST MODE ===" << RESET << "\n";
                    std::cout << GRAY << ITALIC << "    Results won't be saved.\n" << RESET << "\n";

                    int diff;
                    std::cout << BRIGHT_YELLOW << "    1. " << RESET << WHITE << "Easy\n";
                    std::cout << BRIGHT_YELLOW << "    2. " << RESET << WHITE << "Medium\n";
                    std::cout << BRIGHT_YELLOW << "    3. " << RESET << WHITE << "Hard\n";
                    std::cout << RESET << "\n";
                    std::cout << BOLD << BRIGHT_YELLOW << "    Choose difficulty: " << RESET;
                    std::cin >> diff;
                    std::cin.ignore();

                    if (diff < 1 || diff > 3) {
                        std::cout << YELLOW << "    Invalid choice. Defaulting to Easy.\n" << RESET;
                        diff = 1;
                    }

                    std::cout << "\n" << GRAY << "    Press Enter to start..." << RESET;
                    std::cin.get();

                    runSpeedTest(false, diff);

                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n" << GREEN << "    Guest session completed.\n" << RESET;
                    std::cout << GRAY << "    Press Enter to continue..." << RESET;
                    std::cin.get();
                    break;
                }

                case 4: // Exit
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n" << BOLD << CYAN << "    Goodbye! Happy typing!\n\n" << RESET;
                    return 0;

                default:
                    std::cout << RED << "\n    Invalid choice. Try again.\n" << RESET;
                    std::cout << GRAY << "    Press Enter to continue..." << RESET;
                    std::cin.get();
            }
        } else {
            // ============ LOGGED IN MENU ============
            std::cout << CLEAR_SCREEN;
            std::cout << "\n";

            initCharMap();
            std::string encryptedUserId = encryptString(padString(user_id, 23));
            std::string user_name = FileHandler::getUserNameFromRegistry(encryptedUserId);

            std::cout << BOLD << CYAN;
            std::cout << "    ╔══════════════════════════════════════╗\n";
            std::cout << "    ║            GAME  MENU                ║\n";
            std::cout << "    ╚══════════════════════════════════════╝\n";
            std::cout << RESET;
            std::cout << WHITE << "    Logged in as: " << BRIGHT_YELLOW << BOLD << decryptString(user_name) << RESET;
            std::cout << GRAY << " (" << user_id << ")" << RESET << "\n\n";

            std::cout << BOLD << MAGNETA << "    -- Career Mode --\n" << RESET;
            std::cout << BRIGHT_YELLOW << "    1. " << RESET << WHITE << "Easy\n";
            std::cout << BRIGHT_YELLOW << "    2. " << RESET << WHITE << "Medium\n";
            std::cout << BRIGHT_YELLOW << "    3. " << RESET << WHITE << "Hard\n\n";

            std::cout << BOLD << MAGNETA << "    -- Fun Modes --\n" << RESET;
            std::cout << BRIGHT_YELLOW << "    4. " << RESET << WHITE << "Falling Words\n";
            std::cout << BRIGHT_YELLOW << "    5. " << RESET << WHITE << "Timed Test (45s)\n";
            std::cout << BRIGHT_YELLOW << "    6. " << RESET << WHITE << "No Vowel Game\n\n";

            std::cout << BOLD << MAGNETA << "    -- Account --\n" << RESET;
            std::cout << BRIGHT_YELLOW << "    7. " << RESET << WHITE << "View Stats\n";
            std::cout << BRIGHT_YELLOW << "    8. " << RESET << WHITE << "Leaderboard\n";
            std::cout << BRIGHT_YELLOW << "    9. " << RESET << WHITE << "Logout\n";
            std::cout << RESET << "\n";

            std::cout << BOLD << BRIGHT_YELLOW << "    Choice: " << RESET;
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
                // ---- CAREER MODE: Easy ----
                case 1: {
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n" << BOLD << GREEN << "    Starting Easy Speed Test...\n" << RESET;
                    std::cout << GRAY << "    Press Enter to begin..." << RESET;
                    std::cin.get();

                    TestResults results = runSpeedTest(true, 1);

                    currentUser.addSessionResult(results.wpm, results.accuracy);

                    std::string modeName = FileHandler::getGameModeString(1);
                    SessionLogger::logSession(encryptedUserId, modeName, 1, results.wpm, results.accuracy);
                    Leaderboard::submitScore(encryptedUserId, 1, results.wpm, results.accuracy);

                    std::cout << GRAY << "\n    Press Enter to return to menu..." << RESET;
                    std::cin.get();
                    break;
                }

                // ---- CAREER MODE: Medium ----
                case 2: {
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n" << BOLD << YELLOW << "    Starting Medium Speed Test...\n" << RESET;
                    std::cout << GRAY << "    Press Enter to begin..." << RESET;
                    std::cin.get();

                    TestResults results = runSpeedTest(true, 2);

                    currentUser.addSessionResult(results.wpm, results.accuracy);

                    std::string modeName = FileHandler::getGameModeString(1);
                    SessionLogger::logSession(encryptedUserId, modeName, 2, results.wpm, results.accuracy);
                    Leaderboard::submitScore(encryptedUserId, 2, results.wpm, results.accuracy);

                    std::cout << GRAY << "\n    Press Enter to return to menu..." << RESET;
                    std::cin.get();
                    break;
                }

                // ---- CAREER MODE: Hard ----
                case 3: {
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n" << BOLD << RED << "    Starting Hard Speed Test...\n" << RESET;
                    std::cout << GRAY << "    Press Enter to begin..." << RESET;
                    std::cin.get();

                    TestResults results = runSpeedTest(true, 3);

                    currentUser.addSessionResult(results.wpm, results.accuracy);

                    std::string modeName = FileHandler::getGameModeString(1);
                    SessionLogger::logSession(encryptedUserId, modeName, 3, results.wpm, results.accuracy);
                    Leaderboard::submitScore(encryptedUserId, 3, results.wpm, results.accuracy);

                    std::cout << GRAY << "\n    Press Enter to return to menu..." << RESET;
                    std::cin.get();
                    break;
                }

                // ---- FUN MODE: Falling Words ----
                case 4: {
                    std::cout << CLEAR_SCREEN;
                    runFallingWords();
                    std::cout << GRAY << "\n    Press Enter to return to menu..." << RESET;
                    std::cin.get();
                    break;
                }

                // ---- FUN MODE: Timed Test ----
                case 5: {
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n" << BOLD << CYAN << "    Starting Timed Test (45 seconds)...\n" << RESET;
                    std::cout << GRAY << "    Press Enter to begin..." << RESET;
                    std::cin.get();

                    runTimeTest();

                    std::cout << GRAY << "\n    Press Enter to return to menu..." << RESET;
                    std::cin.get();
                    break;
                }

                // ---- FUN MODE: No Vowel Game ----
                case 6: {
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n" << BOLD << MAGNETA << "    Starting No Vowel Game...\n" << RESET;
                    std::cout << GRAY << ITALIC << "    Type the text but replace all vowels with space!\n" << RESET;
                    std::cout << GRAY << "    Press Enter to begin..." << RESET;
                    std::cin.get();

                    vowel_run();

                    std::cout << GRAY << "\n    Press Enter to return to menu..." << RESET;
                    std::cin.get();
                    break;
                }

                // ---- View Stats ----
                case 7: {
                    std::cout << CLEAR_SCREEN;
                    currentUser.displayStats();
                    std::cout << GRAY << "\n    Press Enter to continue..." << RESET;
                    std::cin.get();
                    break;
                }

                // ---- Leaderboard ----
                case 8: {
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n";
                    std::cout << BOLD << CYAN << "    === LEADERBOARD ===" << RESET << "\n\n";

                    int lbDiff;
                    std::cout << BRIGHT_YELLOW << "    1. " << RESET << WHITE << "Easy\n";
                    std::cout << BRIGHT_YELLOW << "    2. " << RESET << WHITE << "Medium\n";
                    std::cout << BRIGHT_YELLOW << "    3. " << RESET << WHITE << "Hard\n";
                    std::cout << RESET << "\n";
                    std::cout << BOLD << BRIGHT_YELLOW << "    Choose difficulty: " << RESET;
                    std::cin >> lbDiff;
                    std::cin.ignore();

                    if (lbDiff < 1 || lbDiff > 3) {
                        std::cout << YELLOW << "    Invalid choice. Defaulting to Easy.\n" << RESET;
                        lbDiff = 1;
                    }

                    std::cout << "\n";
                    Leaderboard::displayLeaderboard(lbDiff);

                    std::cout << GRAY << "\n    Press Enter to continue..." << RESET;
                    std::cin.get();
                    break;
                }

                // ---- Logout ----
                case 9: {
                    isLoggedIn = false;
                    std::cout << CLEAR_SCREEN;
                    std::cout << "\n" << GREEN << "    Logged out successfully.\n" << RESET;
                    std::cout << GRAY << "    Press Enter to continue..." << RESET;
                    std::cin.get();
                    break;
                }

                default:
                    std::cout << RED << "\n    Invalid choice. Try again.\n" << RESET;
                    std::cout << GRAY << "    Press Enter to continue..." << RESET;
                    std::cin.get();
            }
        }
    }

    return 0;
}

// #include "TerminalSetup.h"
// #include "SpeedTest.h"
// #include <iostream>
// #include "user_manager.h"
// #include "user.h"
// #include <string>
// #include <file_helper.h>
// #include <hashing.h>
// #include "session_logger.h"
// #include "leaderboard.h"
// #include "falling_words.h"
// #include "TimeTest.h"
// #include "vowel_game.h"
// int main(){
//     std::cout<<CLEAR_SCREEN;
//     std::cout<<"\n";
//     std:: cout<< BG_BRIGHT_RED<<BRIGHT_WHITE;
//     std::cout << "              === TYPING SPEED TEST APPLICATION ===                                  \n";
//     std::cout <<RESET;
//     std::cout<<"\n";
//     int choice;
//     std::string name, user_id, password;
//     User currentUser;
//     bool isLoggedIn = false;
    
//     while (true) {
//         if (!isLoggedIn) {
//             std::cout<<BRIGHT_MAGENTA;
//             std::cout << "\n            === MAIN MENU ===\n";
//             std::cout << RESET;
//             std::cout << CYAN;
//             std::cout << "1. Register\n2. Login\n3. Play as Guest\n4. Exit\n";
//             std::cout<<RESET;
//             std::cout<<ITALIC<<BRIGHT_YELLOW<<BOLD;
//             std::cout << "Choice: ";
//             std::cout << RESET;
//             std::cin >> choice;
//             std::cin.ignore(); // Clear newline
            
//             switch (choice) {
//                 case 1: { // Register
//                     std::cout << "Enter name: ";
//                     std::getline(std::cin, name);
//                     std::cout << "Enter user ID: ";
//                     std::getline(std::cin, user_id);
//                     std::cout << "Enter password: ";
//                     std::getline(std::cin, password);
                    
//                     if (UserManager::registerUser(name, user_id, password)) {
//                         initCharMap();
//                         std::string encryptedUserId = encryptString(padString(user_id, 23));
                        
//                         // Load the newly registered user
//                         currentUser = UserManager::loadUser(encryptedUserId);
//                         currentUser.setUserId(encryptedUserId);
                        
//                         isLoggedIn = true;
//                         std::cout << "\033[2J\033[H";
//                         std::cout << "\nRegistration successful! Logged in as " << user_id << "\n";
//                     }
//                     break;
//                 }
                    
//                 case 2: { // Login
//                     std::cout << "Enter user ID: ";
//                     std::getline(std::cin, user_id);
//                     std::cout << "Enter password: ";
//                     std::getline(std::cin, password);
                    
//                     if (UserManager::loginUser(user_id, password)) {
//                         initCharMap();
//                         std::string encryptedUserId = encryptString(padString(user_id, 23));
                        
//                         // Load the user with encrypted ID and stats
//                         currentUser = UserManager::loadUser(encryptedUserId);
//                         currentUser.setUserId(encryptedUserId);
                        
//                         std::string user_name = FileHandler::getUserNameFromRegistry(encryptedUserId);
//                         isLoggedIn = true;
//                         std::cout << "\033[2J\033[H";
//                         std::cout << "\nLogin successful! Welcome " << decryptString(user_name) << "!\n";
//                     }
//                     break;
//                 }
                    
//                 case 3: { // Play as Guest
//                     std::cout << "\n=== GUEST MODE ===\n";
//                     std::cout << "Playing as Guest (results won't be saved)...\n";
//                     int diff;
//                     std::cout << "\nChoose difficulty:\n";
//                     std::cout << "1. Easy\n2. Medium\n3. Hard\n";
//                     std::cout << "Choice: ";
//                     std::cin >> diff;
//                     std::cin.ignore();

//                     if (diff < 1 || diff > 3) {
//                         std::cout << "Invalid choice. Defaulting to Easy.\n";
//                         diff = 1;
//                     }
//                     std::cout << "Press Enter to start...";
//                     std::cin.get();
                    
//                     // Run test, don't save (saveToUser = false)
//                     runSpeedTest(false,diff);
                    
//                     std::cout << "\nGuest session completed.\n";
//                     break;
//                 }
                    
//                 case 4: // Exit
//                     std::cout << "\nGoodbye!\n";
//                     return 0;
                    
//                 default:
//                     std::cout << "Invalid choice.\n";
//             }
//         } else {
//             // User is logged in - show game menu
//             std::cout << "\n=== GAME MENU ===\n";
//             initCharMap();
//             std::string encryptedUserId = encryptString(padString(user_id, 23));
//             std::string user_name = FileHandler::getUserNameFromRegistry(encryptedUserId);
//             std::cout << "Logged in as: " << decryptString(user_name) << " (" << user_id << ")\n";
//             std::cout << "1. Start Typing Test\n";
//             std::cout << "2. Start Falling Words\n";
//             std::cout << "3. Start Time Test\n";
//             std::cout << "4. Start Vowel Game\n";
//             std::cout << "5. View Stats\n";
//             std::cout << "6. View Leaderboard\n";
//             std::cout << "7. Logout\n";
//             std::cout << "Choice: ";
//             std::cin >> choice;
//             std::cin.ignore();
            
//             switch (choice) {
//                 case 1: { // Start Typing Test
//                     std::cout << "\nStarting typing test...\n";

//                     int diff;
//                     std::cout << "\nChoose difficulty:\n";
//                     std::cout << "1. Easy\n2. Medium\n3. Hard\n";
//                     std::cout << "Choice: ";
//                     std::cin >> diff;
//                     std::cin.ignore();

//                     if (diff < 1 || diff > 3) {
//                         std::cout << "Invalid choice. Defaulting to Easy.\n";
//                         diff = 1;
//                     }
//                     std::cout << "Press Enter to begin...";
//                     std::cin.get();
                    
//                     // Run test with saveToUser = true (but we manually save)
//                     TestResults results = runSpeedTest(true,diff);
                    
//                     // Update user stats in memory
//                     currentUser.addSessionResult(results.wpm, results.accuracy);
                    
//                     // Using SessionLogger::logSession instead of UserManager::saveUser() here, as it would overwrite
            
//                     std::string modeName = FileHandler::getGameModeString(1);
//                     SessionLogger::logSession(encryptedUserId, modeName, diff, results.wpm, results.accuracy);
                    
//                     // Submit to leaderboard
//                     Leaderboard::submitScore(encryptedUserId, diff, results.wpm, results.accuracy);
                    
//                     // Ask to continue
//                     std::cout << "\nPress Enter to return to menu...";
//                     std::cin.get();
//                     break;
//                 }

//                 case 2:
//                     runFallingWords();
//                     std::cout << "\nPress Enter to return to menu...";
//                     std::cin.get();
//                     break;
                    
//                 case 3: // Start Time Test
//                     runTimeTest();
//                     std::cout << "\nPress Enter to return to menu...";
//                     std::cin.get();
//                     break;
                    
//                 case 4: 
//                     vowel_run();
//                     std::cout << "\nPress Enter to return to menu...";
//                     std::cin.get();
//                     break;
//                 case 5:  //view stats
//                     currentUser.displayStats();
//                     std::cout << "\nPress Enter to continue...";
//                     std::cin.get();
//                     break;
                    
//                 case 6: {// Logout
//                     int lbDiff;
//                     std::cout << "\nChoose leaderboard:\n";
//                     std::cout << "1. Easy\n2. Medium\n3. Hard\n";
//                     std::cout << "Choice: ";
//                     std::cin >> lbDiff;
//                     std::cin.ignore();
//                     if (lbDiff < 1 || lbDiff > 3) lbDiff = 1;
//                     Leaderboard::displayLeaderboard(lbDiff);
//                     std::cout << "\nPress Enter to continue...";
//                     std::cin.get();
//                     break;
//                 }

//                 case 7:
//                     isLoggedIn = false;
//                     std::cout << "\nLogged out successfully.\n";
//                     break;
//                 default:
//                     std::cout << "Invalid choice.\n";
//             }
//         }
//     }
    
//     return 0;
// }