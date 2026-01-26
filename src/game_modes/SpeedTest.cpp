#include "SpeedTest.h"
#include "TerminalSetup.h"
#include <iostream>
#include <../utils/file_helper.h>

using namespace std;

// Helper function to display the typing screen (not in header, only used internally)
void displayScreen(const char reference[], char userInput[], int userLen, int refLen,
                    double timePassed, int totalMistakes, bool timerStarted){
    
    clearScreen();
    cout << BOLD << WHITE << "=== TYPING SPEED TEST ===" << RESET << endl;
    
    // Stats line
    if (timerStarted) {
        int sec = (int)timePassed;
        int dec = ((int)(timePassed * 10)) % 10;
        cout << "Time: " << GREEN << sec << "." << dec << "s" << RESET;
        cout << "  |  Mistakes: " << RED << totalMistakes << RESET;
        if (timePassed > 0.5 && userLen > 0) {
            int wpm = (int)((userLen / 5.0) / (timePassed / 60.0));
            cout << "  |  WPM: " << YELLOW << wpm << RESET;
        }
        cout << endl;
    } else {
        cout << GRAY << "Start typing to begin..." << RESET << endl;
    }
    
    cout << endl;
    cout << GRAY << "[BACKSPACE = correct]  [ENTER = finish]" << RESET << endl;
    cout << endl;
    
    // Reference text
    cout << "Text: " << CYAN << reference << RESET << endl;
    cout << endl;
    
    // User typing with cursor
    cout << "You:  ";
    for (int i = 0; i < userLen; i++) {
        if (i < refLen) {
            if (userInput[i] == reference[i]) {
                cout << BRIGHT_YELLOW << userInput[i] << RESET;
            } else {
                cout << BRIGHT_RED << BOLD << userInput[i] << RESET;
            }
        } else {
            cout << RED << userInput[i] << RESET;
        }
    }

    // Cursor
    if (userLen < refLen) {
        cout << BG_BRIGHT_MAGENTA<<BOLD << BLACK << reference[userLen] << RESET;
        cout << GRAY;
        for (int i = userLen + 1; i < refLen; i++) {
            cout << reference[i];
        }
        cout << RESET;
    }
    cout << endl;
    
    // Progress
    cout << endl;
    cout << "Progress: " << userLen << "/" << refLen << " characters" << endl;
    
    cout.flush();
}


// NEW: Simple, colorful results display
void displayResults(const TestResults& results) {
    clearScreen();
    
    cout << endl;
    cout << BOLD << CYAN << "  ================================" << RESET << endl;
    cout << BOLD << CYAN << "       TYPING TEST RESULTS       " << RESET << endl;
    cout << BOLD << CYAN << "  ================================" << RESET << endl;
    cout << endl;
    
    // Time
    int timeSec = (int)results.time_taken;
    int timeDec = ((int)(results.time_taken * 10)) % 10;
    cout << "  " << WHITE << "Time:        " << RESET << GREEN << timeSec << "." << timeDec << " seconds" << RESET << endl;
    
    // WPM with color based on performance
    cout << "  " << WHITE << "Speed:       " << RESET;
    if (results.wpm >= 60) cout << GREEN << BOLD;
    else if (results.wpm >= 40) cout << YELLOW;
    else cout << RED;
    cout << results.wpm << " WPM" << RESET << endl;
    
    // Accuracy with color
    cout << "  " << WHITE << "Accuracy:    " << RESET;
    if (results.accuracy >= 95) cout << GREEN << BOLD;
    else if (results.accuracy >= 80) cout << YELLOW;
    else cout << RED;
    cout << results.accuracy << "%" << RESET << endl;
    
    // Characters
    cout << "  " << WHITE << "Characters:  " << RESET << CYAN << results.correct_chars << "/" << results.total_chars << RESET << endl;
    
    // Keystrokes
    cout << "  " << WHITE << "Keystrokes:  " << RESET << WHITE << results.keystrokes << RESET << endl;
    
    // Mistakes
    cout << "  " << WHITE << "Mistakes:    " << RESET;
    if (results.mistakes == 0) cout << GREEN << "0 (Perfect!)" << RESET;
    else cout << RED << results.mistakes << RESET;
    cout << endl;
    
    cout << endl;
    cout << BOLD << CYAN << "  ================================" << RESET << endl;
    cout << endl;
    
    // Performance message based on WPM and accuracy
    if (results.accuracy >= 95 && results.wpm >= 60) {
        cout << GREEN << "  Excellent! You're a typing master!" << RESET << endl;
    } else if (results.accuracy >= 85 && results.wpm >= 40) {
        cout << YELLOW << "  Good job! Keep practicing!" << RESET << endl;
    } else {
        cout << CYAN << "  Keep going! Practice makes perfect!" << RESET << endl;
    }
    
    cout << endl;
    cout << "  " << GRAY << "Press " << YELLOW << "'r'" << GRAY << " to retry or " << YELLOW << "'q'" << GRAY << " to quit" << RESET << endl;
    cout << endl;
    
    cout.flush();
}


// UNIFIED: Single function to run speed test
// saveToUser: true = save to user profile, false = guest mode (don't save)
TestResults runSpeedTest(bool saveToUser) {
    // Load reference text
    string text = FileHandler::readFile("../data/texts/practice.txt");
    const char* referenceText = text.c_str();
    int refLen = getLength(referenceText);
    
    char userInput[500];
    int userLen = 0;
    
    setTerminal();
    
    TestResults results = {0, 0, 0.0, 0, 0, 0, 0};
    bool keepPlaying = true;
    
    while (keepPlaying) {
        // Reset for each session
        userLen = 0;
        userInput[0] = '\0';
        int totalMistakes = 0;
        int totalKeystrokes = 0;
        double startTime = 0;
        double timePassed = 0;
        bool timerStarted = false;
        
        displayScreen(referenceText, userInput, userLen, refLen, 0, 0, false);
        
        // Typing loop
        bool typing = true;
        while (typing) {
            // Update timer
            if (timerStarted) {
                timePassed = getCurrentTime() - startTime;
                displayScreen(referenceText, userInput, userLen, refLen,
                                   timePassed, totalMistakes, timerStarted);
            }
            
            // Check for keypress
            if (isKeyPressed()) {
                char ch = readKey();
                
                // Start timer on first printable char
                if (!timerStarted && ch >= 32 && ch <= 126) {
                    startTime = getCurrentTime();
                    timerStarted = true;
                }
                
                if (ch == '\n' || ch == '\r') {
                    // ENTER pressed - END typing
                    typing = false;
                }
                else if (ch == 127 || ch == 8) {
                    // Backspace
                    if (userLen > 0) {
                        userLen--;
                        userInput[userLen] = '\0';
                    }
                }
                else if (ch >= 32 && ch <= 126) {
                    // Printable character
                    totalKeystrokes++;
                    
                    if (userLen < refLen) {
                        if (ch != referenceText[userLen]) {
                            totalMistakes++;
                        }
                    } else {
                        totalMistakes++;
                    }
                    
                    if (userLen < 499) {
                        userInput[userLen] = ch;
                        userLen++;
                        userInput[userLen] = '\0';
                    }
                }
                
                displayScreen(referenceText, userInput, userLen, refLen,
                                   timePassed, totalMistakes, timerStarted);
            }
        }
        
        // Ensure minimum time
        if (timePassed < 1) timePassed = 1;
        
        // Calculate results
        int correct = 0;
        int checkLen = (userLen < refLen) ? userLen : refLen;
        
        for (int i = 0; i < checkLen; i++) {
            if (userInput[i] == referenceText[i]) {
                correct++;
            }
        }
        
        // Populate results struct
        results.time_taken = timePassed;
        results.mistakes = totalMistakes;
        results.keystrokes = totalKeystrokes;
        results.correct_chars = correct;
        results.total_chars = refLen;
        
        // Calculate WPM
        if (timePassed > 0) {
            results.wpm = (int)((userLen / 5.0) / (timePassed / 60.0));
        }
        
        // Calculate accuracy
        if (totalKeystrokes > 0) {
            int correctKeys = totalKeystrokes - totalMistakes;
            if (correctKeys < 0) correctKeys = 0;
            results.accuracy = (correctKeys * 100) / totalKeystrokes;
        }
        
        // Display results using the new function
        displayResults(results);
        
        // Wait for user choice (both guest and user can retry!)
        char choice = waitForKey();
        
        if (choice == 'q' || choice == 'Q') {
            keepPlaying = false;
        }
        // If 'r' or any other key, loop continues and test restarts
    }
    
    restoreTerminal();
    clearScreen();
    
    // Return results (will be saved only if saveToUser is true in main.cpp)
    return results;
}