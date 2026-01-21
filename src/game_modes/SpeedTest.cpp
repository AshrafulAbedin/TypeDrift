#include "SpeedTest.h"
#include "TerminalSetup.h"
#include <iostream>
#include <../utils/file_helper.h>

using namespace std;

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
                cout << YELLOW << userInput[i] << RESET;
            } else {
                cout << RED << BOLD << userInput[i] << RESET;
            }
        } else {
            cout << RED << userInput[i] << RESET;
        }
    }

    // Cursor
    if (userLen < refLen) {
        cout << BG_WHITE << BLACK << reference[userLen] << RESET;
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
    
    cout. flush();

}

// ============== RESULTS SCREEN - THIS WILL DEFINITELY SHOW!  ==============
void showResults(const char reference[], char userInput[], int userLen, int refLen,
                 double timeTaken, int totalMistakes, int totalKeystrokes) {
    
    clearScreen();
    
    // ===== CALCULATIONS =====
    int correct = 0;
    int checkLen = (userLen < refLen) ? userLen : refLen;
    
    for (int i = 0; i < checkLen; i++) {
        if (userInput[i] == reference[i]) {
            correct++;
        }
    }
    
    // Final accuracy (based on final text)
    int finalAccuracy = 0;
    if (refLen > 0) {
        finalAccuracy = (correct * 100) / refLen;
    }
    
    // True accuracy (counts all mistakes)
    int trueAccuracy = 0;
    if (totalKeystrokes > 0) {
        int correctKeys = totalKeystrokes - totalMistakes;
        if (correctKeys < 0) correctKeys = 0;
        trueAccuracy = (correctKeys * 100) / totalKeystrokes;
    }
    
    // WPM
    int wpm = 0;
    if (timeTaken > 0) {
        wpm = (int)((userLen / 5.0) / (timeTaken / 60.0));
    }
    
    // Time with decimal
    int timeSec = (int)timeTaken;
    int timeDec = ((int)(timeTaken * 10)) % 10;
    
    // ===== DISPLAY RESULTS =====
    
    cout << endl;
    cout << BOLD << WHITE;
    cout << "  ╔═══════════════════════════════════════╗" << endl;
    cout << "  ║         TYPING TEST RESULTS           ║" << endl;
    cout << "  ╚═══════════════════════════════════════╝" << endl;
    cout << RESET << endl;
    
    // Reference vs User text
    cout << "  " << WHITE << "Reference:" << RESET << endl;
    cout << "  " << CYAN << reference << RESET << endl;
    cout << endl;
    
    cout << "  " << WHITE << "You typed:" << RESET << endl;
    cout << "  ";
    for (int i = 0; i < userLen; i++) {
        if (i < refLen && userInput[i] == reference[i]) {
            cout << YELLOW << userInput[i] << RESET;
        } else {
            cout << RED << userInput[i] << RESET;
        }
    }
    cout << endl;
    cout << endl;
    
    // Stats Box
    cout << "  " << BOLD << "┌─────────────────────────────────────┐" << RESET << endl;
    cout << "  " << BOLD << "│" << RESET << "           YOUR STATISTICS           " << BOLD << "│" << RESET << endl;
    cout << "  " << BOLD << "├─────────────────────────────────────┤" << RESET << endl;
    
    // Time
    cout << "  " << BOLD << "│" << RESET;
    cout << "  ⏱  Time:           " << GREEN << BOLD << timeSec << "." << timeDec << " seconds" << RESET;
    cout << "      " << BOLD << "│" << RESET << endl;
    
    // WPM
    cout << "  " << BOLD << "│" << RESET;
    cout << "  ⌨  WPM:            " << YELLOW << BOLD << wpm << RESET;
    cout << "                  " << BOLD << "│" << RESET << endl;
    
    // Final Accuracy
    cout << "  " << BOLD << "│" << RESET;
    cout << "  ✓  Final Accuracy: ";
    if (finalAccuracy >= 95) cout << GREEN << BOLD;
    else if (finalAccuracy >= 80) cout << YELLOW << BOLD;
    else cout << RED << BOLD;
    cout << finalAccuracy << "%" << RESET;
    cout << "                 " << BOLD << "│" << RESET << endl;
    
    // True Accuracy
    cout << "  " << BOLD << "│" << RESET;
    cout << "  ★  True Accuracy:  ";
    if (trueAccuracy >= 95) cout << GREEN << BOLD;
    else if (trueAccuracy >= 80) cout << YELLOW << BOLD;
    else cout << RED << BOLD;
    cout << trueAccuracy << "%" << RESET;
    cout << "                 " << BOLD << "│" << RESET << endl;
    
    // Correct chars
    cout << "  " << BOLD << "│" << RESET;
    cout << "  📝 Correct:        " << GREEN << correct << "/" << refLen << " chars" << RESET;
    cout << "           " << BOLD << "│" << RESET << endl;
    
    // Total keystrokes
    cout << "  " << BOLD << "│" << RESET;
    cout << "  🔢 Keystrokes:     " << WHITE << totalKeystrokes << RESET;
    cout << "                  " << BOLD << "│" << RESET << endl;
    
    // Mistakes
    cout << "  " << BOLD << "│" << RESET;
    cout << "  ✗  Mistakes:       " << RED << totalMistakes << RESET;
    cout << "                  " << BOLD << "│" << RESET << endl;
    
    cout << "  " << BOLD << "└─────────────────────────────────────┘" << RESET << endl;
    cout << endl;
    
    // Explanation
    cout << "  " << GRAY << "Final Accuracy = Your final text vs reference" << RESET << endl;
    cout << "  " << GRAY << "True Accuracy  = Counts all mistakes (even corrected)" << RESET << endl;
    cout << endl;
    
    // Mistakes detail
    cout << "  " << WHITE << "Mistakes in final text: " << RESET;
    bool hasMistake = false;
    for (int i = 0; i < checkLen; i++) {
        if (userInput[i] != reference[i]) {
            cout << "'" << reference[i] << "'->'" << userInput[i] << "' ";
            hasMistake = true;
        }
    }
    if (! hasMistake) {
        cout << GREEN << "None!  Perfect!" << RESET;
    }
    cout << endl;
    
    if (totalMistakes > 0 && !hasMistake) {
        cout << "  " << GRAY << "(You corrected " << totalMistakes << " mistake(s) with backspace)" << RESET << endl;
    }
    
    cout << endl;
    cout << "  " << BOLD << WHITE << "═══════════════════════════════════════" << RESET << endl;
    cout << endl;
    cout << "  " << WHITE << "Press " << YELLOW << "'r'" << WHITE << " to retry or " << YELLOW << "'q'" << WHITE << " to quit: " << RESET;
    cout. flush();
}


//main function
void runSpeedTest(){
    string text = FileHandler :: readFile("../data/texts/practice.txt");
    const char* referenceText = text.c_str();
    int refLen = getLength(referenceText);
    
    char userInput[500];
    int userLen = 0;
    
    setTerminal();
    
    bool keepPlaying = true;
    
    while (keepPlaying) {
        // Reset
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
                if (! timerStarted && ch >= 32 && ch <= 126) {
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
        
        // Final time
        if (timePassed < 1) timePassed = 1;
        
        // ===== SHOW RESULTS!  =====
        showResults(referenceText, userInput, userLen, refLen,
                   timePassed, totalMistakes, totalKeystrokes);
        
        // Wait for user to press a key
        char choice = waitForKey();
        
        if (choice == 'q' || choice == 'Q') {
            keepPlaying = false;
        }
        // Otherwise (including 'r'), restart
    }
    
    restoreTerminal();
    clearScreen();
    cout << "Thanks for practicing!  Goodbye!" << endl;
    
}


// Run speed test V2 which returrns Test Results
TestResults runSpeedTestWithResults(){
    string text = FileHandler :: readFile("../data/texts/practice.txt");
    const char* referenceText = text.c_str();
    int refLen = getLength(referenceText);
    
    char userInput[500];
    int userLen = 0;
    
    setTerminal();
    
    // Variables for results
    TestResults results = {0, 0, 0, 0, 0};
    int totalMistakes = 0;
    int totalKeystrokes = 0;
    double startTime = 0;
    double timePassed = 0;
    bool timerStarted = false;
    bool keepPlaying = true;
    
    while (keepPlaying) {
        // Reset for each session
        userLen = 0;
        userInput[0] = '\0';
        totalMistakes = 0;
        totalKeystrokes = 0;
        startTime = 0;
        timePassed = 0;
        timerStarted = false;
        
        displayScreen(referenceText, userInput, userLen, refLen, 0, 0, false);
        
        // Typing loop
        bool typing = true;
        while (typing) {
            if (timerStarted) {
                timePassed = getCurrentTime() - startTime;
                displayScreen(referenceText, userInput, userLen, refLen,
                                   timePassed, totalMistakes, timerStarted);
            }
            
            if (isKeyPressed()) {
                char ch = readKey();
                
                if (!timerStarted && ch >= 32 && ch <= 126) {
                    startTime = getCurrentTime();
                    timerStarted = true;
                }
                
                if (ch == '\n' || ch == '\r') {
                    typing = false;
                }
                else if (ch == 127 || ch == 8) {
                    if (userLen > 0) {
                        userLen--;
                        userInput[userLen] = '\0';
                    }
                }
                else if (ch >= 32 && ch <= 126) {
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
        
        // Final time
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
        
        // Calculate WPM
        results.wpm = (int)((userLen / 5.0) / (timePassed / 60.0));
        
        // Calculate accuracy
        if (totalKeystrokes > 0) {
            int correctKeys = totalKeystrokes - totalMistakes;
            if (correctKeys < 0) correctKeys = 0;
            results.accuracy = (correctKeys * 100) / totalKeystrokes;
        }
        
        // Show results
        showResults(referenceText, userInput, userLen, refLen,
                   timePassed, totalMistakes, totalKeystrokes);
        
        // Wait for user to press a key
        char choice = waitForKey();
        
        if (choice == 'q' || choice == 'Q') {
            keepPlaying = false;
        }
        // 'r' will restart loop
    }
    
    restoreTerminal();
    clearScreen();
    
    return results;  // Return the results

    
}