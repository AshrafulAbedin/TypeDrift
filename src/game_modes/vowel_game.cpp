#include "vowel_game.h"
#include "TerminalSetup.h"
#include <iostream>
#include "file_helper.h"
#include <cstdlib> // For rand

using namespace std;

// Struct for falling animation particles
struct VowelParticle {
    char c;
    double x;
    double y;
    double vy; // velocity
    bool active;
};
const int MAX_PARTICLES = 30;

// Helper function to check if a character is a vowel
bool isVowel(char c) {
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
            c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
}

// Helper to check if the user typed correctly
bool isCorrectVG(char user_c, char ref_c) {
    if (isVowel(ref_c)) {
        return user_c == ' ';
    }
    return user_c == ref_c;
}

// Helper function to display the typing screen
void displayScreenVG(const char reference[], char userInput[], int userLen, int refLen,
                    double timePassed, int totalMistakes, bool timerStarted, VowelParticle particles[]){
    
    clearScreen();
    cout << BOLD << WHITE << "=== VOWEL GAME ===" << RESET << endl;
    cout << GRAY << "Type the text, but press SPACE BAR instead of any vowel!" << RESET << endl;
    
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
            if (isCorrectVG(userInput[i], reference[i])) {
                cout << BRIGHT_YELLOW << reference[i] << RESET; // show the actual character not space
            } else {
                cout << BRIGHT_RED << BOLD << reference[i] << RESET;
            }
        } else {
            cout << RED << reference[i] << RESET;
        }
    }

    // Cursor - changed from block cursor to underline to remove the 'blue box'
    if (userLen < refLen) {
        cout << UNDERLINE << BRIGHT_CYAN << reference[userLen] << RESET;
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
    int percent = refLen > 0 ? (userLen * 100) / refLen : 0;
    cout << "Progress: " << percent << " % " << endl;
    cout << endl;

    // Render falling tiles (vowels) animation area below progress
    char animBuffer[6][80];
    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 80; c++) animBuffer[r][c] = ' ';
    }
    
    for (int p = 0; p < MAX_PARTICLES; p++) {
        if (particles[p].active) {
            int r = (int)particles[p].y;
            int c = (int)particles[p].x;
            if (r >= 0 && r < 6 && c >= 0 && c < 80) {
                 animBuffer[r][c] = particles[p].c;
            }
        }
    }
    
    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 80; c++) {
            if (animBuffer[r][c] != ' ') {
                char ch = animBuffer[r][c];
                if (ch == 'a' || ch == 'A') cout << BRIGHT_CYAN << BOLD << ch << RESET;
                else if (ch == 'e' || ch == 'E') cout << BRIGHT_GREEN << BOLD << ch << RESET;
                else if (ch == 'i' || ch == 'I') cout << BRIGHT_YELLOW << BOLD << ch << RESET;
                else if (ch == 'o' || ch == 'O') cout << BRIGHT_MAGENTA << BOLD << ch << RESET;
                else cout << BRIGHT_RED << BOLD << ch << RESET;
            } else {
                cout << ' ';
            }
        }
        cout << endl;
    }

    cout.flush();
}

void displayResultsVG(const TestResults& results) {
    clearScreen();
    
    cout << endl;
    cout << BOLD << CYAN << "  ================================" << RESET << endl;
    cout << BOLD << CYAN << "         VOWEL GAME RESULTS       " << RESET << endl;
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
        cout << GREEN << "  Excellent! You are a vowel master >_<!" << RESET << endl;
    } else if (results.accuracy >= 85 && results.wpm >= 40) {
        cout << YELLOW << "  Good job! Keep practicing ! ;-;" << RESET << endl;
    } else {
        cout << CYAN << "  Keep going! You should do better -.-" << RESET << endl;
    }
    
    cout << endl;
    cout << "  " << GRAY << "Press any key to return to menu..." << RESET << endl;
    cout << endl;
    
    cout.flush();
}

TestResults vowel_run() {

    setTerminal();
    
    TestResults results = {0, 0, 0.0, 0, 0, 0, 0, 0};

    // Pick a random file index from 1 to 50
    long current_time = (long)getCurrentTime();
    srand((unsigned)current_time);
    int fileindex = (current_time % 50) + 1;

    // Build the folder name
    char filename[100];
    const char* folder = "../data/texts/no_vowel_txt/n_vwl";
    
    int i = 0;
    while (folder[i] != '\0') {
        filename[i] = folder[i];
        i++;
    }

    // Append the file number (1-50)
    if (fileindex >= 10) {
        filename[i++] = '0' + (fileindex / 10);
    }
    filename[i++] = '0' + (fileindex % 10);

    // Append .txt
    filename[i++] = '.';
    filename[i++] = 't';
    filename[i++] = 'x';
    filename[i++] = 't';
    filename[i] = '\0';
    
    // Load reference text
    string text = FileHandler::readFile(filename);
    const char* referenceText = text.c_str();
    int refLen = getLength(referenceText);
    
    char userInput[2000]; // Make it slightly larger just in case
    int userLen = 0;
        
    // Reset for each session
    userLen = 0;
    userInput[0] = '\0';
    int totalMistakes = 0;
    int totalKeystrokes = 0;
    double startTime = 0;
    double timePassed = 0;
    bool timerStarted = false;

    // Setup animation particles
    VowelParticle particles[MAX_PARTICLES];
    for(int p=0; p<MAX_PARTICLES; p++) particles[p].active = false;
    
    displayScreenVG(referenceText, userInput, userLen, refLen, 0, 0, false, particles);
    
    // Typing loop
    bool typing = true;
    while (typing) {
        // Update timer
        if (timerStarted) {
            timePassed = getCurrentTime() - startTime;
        }

        // Update particle physics (~50ms elapsed per block)
        if (timerStarted) {
            double dt = 0.05; 
            for (int p = 0; p < MAX_PARTICLES; p++) {
                if (particles[p].active) {
                    particles[p].y += particles[p].vy * dt;
                    particles[p].vy += 12.0 * dt; // gravity
                    if (particles[p].y > 6.0) particles[p].active = false; // Despawn
                }
            }
            displayScreenVG(referenceText, userInput, userLen, refLen,
                               timePassed, totalMistakes, timerStarted, particles);
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
                    if (!isCorrectVG(ch, referenceText[userLen])) {
                        totalMistakes++;
                    } else if (isVowel(referenceText[userLen])) {
                        // Vowel accurately hit! Pop it off!
                        for (int p = 0; p < MAX_PARTICLES; p++) {
                            if (!particles[p].active) {
                                particles[p].active = true;
                                particles[p].c = referenceText[userLen];
                                particles[p].x = (userLen % 60) + 10; // keep inside roughly
                                particles[p].y = 0.0;
                                particles[p].vy = -3.0 - ((rand() % 20) / 10.0); // jump up slightly and vary
                                break;
                            }
                        }
                    }
                } else {
                    totalMistakes++;
                }
                
                if (userLen < 1999) {
                    userInput[userLen] = ch;
                    userLen++;
                    userInput[userLen] = '\0';
                }
            }
            
            displayScreenVG(referenceText, userInput, userLen, refLen,
                               timePassed, totalMistakes, timerStarted, particles);
        }
    }
    
    // Ensure minimum time
    if (timePassed < 1) timePassed = 1;
    
    // Calculate results
    int correct = 0;
    int checkLen = (userLen < refLen) ? userLen : refLen;
    
    for (int j = 0; j < checkLen; j++) {
        if (isCorrectVG(userInput[j], referenceText[j])) {
            correct++;
        }
    }
    
    // Populate results struct
    results.time_taken = timePassed;
    results.mistakes = totalMistakes;
    results.keystrokes = totalKeystrokes;
    results.correct_chars = correct;
    results.total_chars = refLen;
    results.chars_typed = userLen;
    
    // Calculate WPM
    if (timePassed > 0) {
        results.wpm = (int)((correct / 5.0) / (timePassed / 60.0));
    }
    
    // Calculate accuracy
    if (refLen > 0) {
        results.accuracy = (correct * 100) / refLen;
    }
    
    displayResultsVG(results);
    
    // Wait for user choice
    waitForKey();
    
    restoreTerminal();
    clearScreen();
    
    return results;
}