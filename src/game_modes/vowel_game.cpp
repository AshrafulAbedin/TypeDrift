#include "vowel_game.h"
#include "TerminalSetup.h"
#include "SpeedTest.h" // For TestResults struct
#include <iostream>
#include <string>
#include <../utils/file_helper.h>
#include <unistd.h> // for read/write if needed, but TerminalSetup abstracts most

using namespace std;

// Helper to check if a character is a vowel
bool isVowel(char c) {
    c = tolower(c);
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

// Helper function to display the typing screen
void vowel_displayScreen(const char reference[], char userInput[], int userLen, int refLen,
                    double timePassed, int totalMistakes, bool timerStarted){
    
    clearScreen();
    cout << BOLD << MAGNETA << "=== NO VOWEL GAME ===" << RESET << endl;
    cout << ITALIC << GRAY << "Type the text normally but REPLACE ALL VOWELS WITH SPACE!" << RESET << endl;
    
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
        cout << GRAY << "Start typing to begin... (Example: 'apple' -> ' ppl ')" << RESET << endl;
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
        char correctChar = isVowel(reference[i]) ? ' ' : reference[i];
        
        if (i < refLen) {
            if (userInput[i] == correctChar) {
                if (correctChar == ' ') {
                    // Highlight space clearly
                    cout << BG_BRIGHT_BLUE << " " << RESET; // Blue background for spaces
                }
            } else {
                // Wrong input
                // If user typed ' ' but it wasn't a vowel spot, or vice versa
                if (reference[i] == ' ') {
                   cout << BRIGHT_RED << userInput[i] << RESET; 
                } else {
                    cout << BRIGHT_RED << BOLD << reference[i] << RESET;
                }
            }
        } else {
            cout << RED << reference[i] << RESET;
        }
    }

    // Cursor
    if (userLen < refLen) {
        cout << BRIGHT_MAGENTA << BOLD << BLACK;
        char next = reference[userLen];
        // Show what should be typed? Or just the original char
        cout << next << RESET;
        
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
    int percent = (refLen > 0) ? (userLen*100)/refLen : 0;
    cout<<"Progress: "<< percent<<" % "<<endl;
    
    cout.flush();
}

void vowel_displayResults(const TestResults& results) {
    clearScreen();
    
    cout << endl;
    cout << BOLD << CYAN << "  ================================" << RESET << endl;
    cout << BOLD << CYAN << "       NO VOWEL GAME RESULTS      " << RESET << endl;
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
    
    cout << endl;
    cout << "  " << GRAY << "Press any key to return to menu..." << RESET << endl;
    cout << endl;
    
    cout.flush();
}

void vowel_run() {
    setTerminal();
    
    TestResults results = {0, 0, 0.0, 0, 0, 0, 0};

    long current_time = (long)getCurrentTime();
    // 25 files available (n_vwl1 to n_vwl25)
    int fileindex = (current_time % 25) + 1;

    // Use requested folder: data/no_vowel_txt
    // Assuming executable is in build/ or root - typically we look for ../data or data
    // SpeedTest uses ../data/texts.
    // Let's try ../data/no_vowel_txt first as it matches SpeedTest structure
    
    char filename[100];
    const char* folder = "../data/no_vowel_txt/n_vwl";

    // Copy folder path into filename
    int i = 0;
    while (folder[i] != '\0') {
        filename[i] = folder[i];
        i++;
    }

    // Append the file number
    string numStr = to_string(fileindex);
    for(char c : numStr) {
        filename[i++] = c;
    }

    // Append .txt
    filename[i++] = '.';
    filename[i++] = 't';
    filename[i++] = 'x';
    filename[i++] = 't';
    filename[i] = '\0';
    
    // Load reference text
    string text = FileHandler::readFile(filename);
    
    // Fallback if file not found
    if (text.empty()) {
        text = "The quick brown fox jumps over the lazy dog"; // Fallback text
    }
    
    const char* referenceText = text.c_str();
    int refLen = getLength(referenceText);
    
    char userInput[500];
    int userLen = 0;
        
    // Reset for each session
    userLen = 0;
    userInput[0] = '\0';
    int totalMistakes = 0;
    int totalKeystrokes = 0;
    double startTime = 0;
    double timePassed = 0;
    bool timerStarted = false;
    
    vowel_displayScreen(referenceText, userInput, userLen, refLen, 0, 0, false);
    
    // Typing loop
    bool typing = true;
    while (typing) {
        // Update timer
        if (timerStarted) {
            timePassed = getCurrentTime() - startTime;
            vowel_displayScreen(referenceText, userInput, userLen, refLen,
                               timePassed, totalMistakes, timerStarted);
        }
        
        // Check for keypress
        if (isKeyPressed()) {
            char ch = readKey();
            
            // Start timer on first printable char (or space)
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
                    // Logic: If ref is vowel, expect space. If ref is consonant, expect consonant.
                    char expectedChar = isVowel(referenceText[userLen]) ? ' ' : referenceText[userLen];
                    
                    if (ch != expectedChar) {
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
            
            vowel_displayScreen(referenceText, userInput, userLen, refLen,
                               timePassed, totalMistakes, timerStarted);
        }
    }
    
    // Ensure minimum time
    if (timePassed < 1) timePassed = 1;
    
    // Calculate results
    int correct = 0;
    int checkLen = (userLen < refLen) ? userLen : refLen;
    
    for (int i = 0; i < checkLen; i++) {
        char expectedChar = isVowel(referenceText[i]) ? ' ' : referenceText[i];
        if (userInput[i] == expectedChar) {
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
        results.wpm = (int)((correct / 5.0) / (timePassed / 60.0));
    }
    
    // Calculate accuracy
    if (refLen > 0) {
        results.accuracy = (correct * 100) / refLen;
    }
    
    vowel_displayResults(results);
    
    // Wait for user choice
    waitForKey();
    
    restoreTerminal();
    clearScreen();
}
