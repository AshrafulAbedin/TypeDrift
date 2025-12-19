#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

// ============== COLORS ==============
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define YELLOW      "\033[33m"
#define GRAY        "\033[90m"
#define BOLD        "\033[1m"
#define GREEN       "\033[32m"
#define CYAN        "\033[36m"
#define WHITE       "\033[97m"
#define BG_WHITE    "\033[47m"
#define BLACK       "\033[30m"

// ============== TERMINAL ==============
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

struct termios originalSettings;

void setupTerminal() {
    struct termios newSettings;
    tcgetattr(STDIN_FILENO, &originalSettings);
    newSettings = originalSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);
    newSettings.c_cc[VMIN] = 0;
    newSettings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

void restoreTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings);
}

bool isKeyPressed() {
    struct timeval tv;
    fd_set fds;
    tv. tv_sec = 0;
    tv.tv_usec = 50000;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

char readKey() {
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
}

// Wait for any key (blocking)
char waitForKey() {
    // Temporarily make input blocking
    struct termios temp;
    tcgetattr(STDIN_FILENO, &temp);
    temp.c_cc[VMIN] = 1;  // Wait for at least 1 character
    temp.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &temp);
    
    char ch;
    read(STDIN_FILENO, &ch, 1);
    
    // Restore non-blocking
    temp.c_cc[VMIN] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &temp);
    
    return ch;
}

void clearScreen() {
    cout << "\033[2J\033[H";
    cout. flush();
}

double getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
}

int getLength(const char text[]) {
    int len = 0;
    while (text[len] != '\0') {
        len++;
    }
    return len;
}

// ============== TYPING SCREEN ==============
void displayTypingScreen(const char reference[], char userInput[], int userLen, int refLen,
                         double elapsedTime, int totalMistakes, bool timerStarted) {
    clearScreen();
    
    cout << BOLD << WHITE << "=== TYPING SPEED TEST ===" << RESET << endl;
    
    // Stats line
    if (timerStarted) {
        int sec = (int)elapsedTime;
        int dec = ((int)(elapsedTime * 10)) % 10;
        cout << "Time: " << GREEN << sec << "." << dec << "s" << RESET;
        cout << "  |  Mistakes: " << RED << totalMistakes << RESET;
        if (elapsedTime > 0.5 && userLen > 0) {
            int wpm = (int)((userLen / 5.0) / (elapsedTime / 60.0));
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

// ============== MAIN ==============
int main() {
    const char referenceText[] = "the quick brown fox jumps over the lazy dog";
    int refLen = getLength(referenceText);
    
    char userInput[500];
    int userLen = 0;
    
    setupTerminal();
    
    bool keepPlaying = true;
    
    while (keepPlaying) {
        // Reset
        userLen = 0;
        userInput[0] = '\0';
        int totalMistakes = 0;
        int totalKeystrokes = 0;
        double startTime = 0;
        double elapsedTime = 0;
        bool timerStarted = false;
        
        displayTypingScreen(referenceText, userInput, userLen, refLen, 0, 0, false);
        
        // Typing loop
        bool typing = true;
        while (typing) {
            // Update timer
            if (timerStarted) {
                elapsedTime = getCurrentTime() - startTime;
                displayTypingScreen(referenceText, userInput, userLen, refLen,
                                   elapsedTime, totalMistakes, timerStarted);
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
                
                displayTypingScreen(referenceText, userInput, userLen, refLen,
                                   elapsedTime, totalMistakes, timerStarted);
            }
        }
        
        // Final time
        if (elapsedTime < 1) elapsedTime = 1;
        
        // ===== SHOW RESULTS!  =====
        showResults(referenceText, userInput, userLen, refLen,
                   elapsedTime, totalMistakes, totalKeystrokes);
        
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
    
    return 0;
}