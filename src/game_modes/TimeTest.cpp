#include "TimeTest.h"
#include "TerminalSetup.h"
#include <iostream>
#include "../utils/file_helper.h"

using namespace std;

const int TIME_LIMIT = 45;

void displayTimeScreen(const char reference[], char userInput[], int userLen, int refLen,
                       double timeLeft, int totalMistakes, bool timerStarted) {

    clearScreen();
    cout << BOLD << WHITE << "=== TIMED TYPING TEST (45s) ===" << RESET << endl;

    if (timerStarted) {
        int sec = (int)timeLeft;

        cout << "Time Left: ";
        if (sec <= 10) cout << RED << BOLD;
        else if (sec <= 20) cout << YELLOW;
        else cout << GREEN;
        cout << sec << "s" << RESET;

        cout << "  |  Mistakes: " << RED << totalMistakes << RESET;

        if (userLen > 0 && timeLeft < TIME_LIMIT) {
            double elapsed = TIME_LIMIT - timeLeft;
            if (elapsed > 0.5) {
                int wpm = (int)((userLen / 5.0) / (elapsed / 60.0));
                cout << "  |  WPM: " << YELLOW << wpm << RESET;
            }
        }
        cout << endl;
    } else {
        cout << GRAY << "Start typing to begin the 45s countdown..." << RESET << endl;
    }

    cout << endl;
    cout << GRAY << "[BACKSPACE = correct]  [ENTER = finish early]" << RESET << endl;
    cout << endl;

    cout << "Text: " << CYAN;
    for (int i = 0; i < refLen; i++) {
        cout << reference[i];
    }
    cout << RESET << endl;
    cout << endl;

    cout << "You:  ";
    for (int i = 0; i < userLen; i++) {
        if (i < refLen) {
            if (userInput[i] == reference[i]) {
                cout << BRIGHT_YELLOW << userInput[i] << RESET;
            } else {
                cout << BRIGHT_RED << BOLD << reference[i] << RESET;
            }
        }
    }

    if (userLen < refLen) {
        cout << BG_BRIGHT_MAGENTA << BOLD << BLACK << reference[userLen] << RESET;
        cout << GRAY;
        for (int i = userLen + 1; i < refLen; i++) {
            cout << reference[i];
        }
        cout << RESET;
    }
    cout << endl;

    cout << endl;
    cout << "Progress: " << userLen << "/" << refLen << " characters" << endl;
    int percent = (userLen * 100) / refLen;
    cout << "Progress: " << percent << " %" << endl;

    cout.flush();
}

void displayTimeResults(const TestResults& results) {

    clearScreen();

    cout << endl;
    cout << BOLD << CYAN << "  ================================" << RESET << endl;
    cout << BOLD << CYAN << "     TIMED TEST RESULTS (45s)    " << RESET << endl;
    cout << BOLD << CYAN << "  ================================" << RESET << endl;
    cout << endl;

    int timeSec = (int)results.time_taken;
    int timeDec = ((int)(results.time_taken * 10)) % 10;
    cout << "  " << WHITE << "Time Used:   " << RESET << GREEN << timeSec << "." << timeDec << " seconds" << RESET << endl;

    cout << "  " << WHITE << "Speed:       " << RESET;
    if (results.wpm >= 60) cout << GREEN << BOLD;
    else if (results.wpm >= 40) cout << YELLOW;
    else cout << RED;
    cout << results.wpm << " WPM" << RESET << endl;

    cout << "  " << WHITE << "Accuracy:    " << RESET;
    if (results.accuracy >= 95) cout << GREEN << BOLD;
    else if (results.accuracy >= 80) cout << YELLOW;
    else cout << RED;
    cout << results.accuracy << "%" << RESET << endl;

    cout << "  " << WHITE << "Characters:  " << RESET << CYAN << results.correct_chars << "/" << results.total_chars << RESET << endl;
    cout << "  " << WHITE << "Keystrokes:  " << RESET << WHITE << results.keystrokes << RESET << endl;

    cout << "  " << WHITE << "Mistakes:    " << RESET;
    if (results.mistakes == 0) cout << GREEN << "0 (Perfect!)" << RESET;
    else cout << RED << results.mistakes << RESET;
    cout << endl;

    cout << endl;
    cout << BOLD << CYAN << "  ================================" << RESET << endl;
    cout << endl;

    if (results.accuracy >= 95 && results.wpm >= 60) {
        cout << GREEN << "  Excellent! Speed demon!" << RESET << endl;
    } else if (results.accuracy >= 85 && results.wpm >= 40) {
        cout << YELLOW << "  Good job! Keep pushing!" << RESET << endl;
    } else {
        cout << CYAN << "  Keep practicing! You'll get faster!" << RESET << endl;
    }

    cout << endl;
    cout << "  " << GRAY << "Press any key to return to menu..." << RESET << endl;
    cout << endl;

    cout.flush();
}

TestResults runTimeTest() {

    setTerminal();

    TestResults results = {0, 0, 0.0, 0, 0, 0, 0};

    // Pick a random file from timed1.txt to timed50.txt
    long current_time = (long)getCurrentTime();
    int fileindex = (current_time % 50) + 1;

    char filename[100];
    const char base_path[] = "../data/texts/medium/medium";
    int fi = 0;
    while (base_path[fi] != '\0') {
        filename[fi] = base_path[fi];
        fi++;
    }
    if (fileindex >= 10) {
        filename[fi++] = '0' + (fileindex / 10);
    }
    filename[fi++] = '0' + (fileindex % 10);
    filename[fi++] = '.';
    filename[fi++] = 't';
    filename[fi++] = 'x';
    filename[fi++] = 't';
    filename[fi] = '\0';

    string text = FileHandler::readFile(filename);
    const char* referenceText = text.c_str();
    int refLen = getLength(referenceText);

    char userInput[2000];
    int userLen = 0;
    userInput[0] = '\0';
    int totalMistakes = 0;
    int totalKeystrokes = 0;
    double startTime = 0;
    double timePassed = 0;
    bool timerStarted = false;

    displayTimeScreen(referenceText, userInput, userLen, refLen,
                      (double)TIME_LIMIT, 0, false);

    bool typing = true;
    while (typing) {

        double timeLeft = (double)TIME_LIMIT;

        if (timerStarted) {
            timePassed = getCurrentTime() - startTime;
            timeLeft = TIME_LIMIT - timePassed;

            if (timeLeft <= 0) {
                timePassed = TIME_LIMIT;
                typing = false;
                break;
            }

            displayTimeScreen(referenceText, userInput, userLen, refLen,
                              timeLeft, totalMistakes, true);
        }

        if (isKeyPressed()) {
            char ch = readKey();

            if (!timerStarted && ch >= 32 && ch <= 126) {
                startTime = getCurrentTime();
                timerStarted = true;
            }

            if (ch == '\n' || ch == '\r') {
                if (timerStarted) {
                    timePassed = getCurrentTime() - startTime;
                }
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

                if (userLen < 1999) {
                    userInput[userLen] = ch;
                    userLen++;
                    userInput[userLen] = '\0';
                }
            }

            if (userLen >= refLen) {
                if (timerStarted) {
                    timePassed = getCurrentTime() - startTime;
                }
                typing = false;
            }

            if (typing && timerStarted) {
                timeLeft = TIME_LIMIT - (getCurrentTime() - startTime);
                if (timeLeft <= 0) {
                    timePassed = TIME_LIMIT;
                    typing = false;
                } else {
                    displayTimeScreen(referenceText, userInput, userLen, refLen,
                                      timeLeft, totalMistakes, true);
                }
            }
        }
    }

    // Calculate results
    if (timerStarted) {
        timePassed = getCurrentTime() - startTime;
        if (timePassed > TIME_LIMIT) timePassed = TIME_LIMIT;
    }
    if (timePassed < 1) timePassed = 1;

    int correct = 0;
    int checkLen = (userLen < refLen) ? userLen : refLen;

    for (int i = 0; i < checkLen; i++) {
        if (userInput[i] == referenceText[i]) {
            correct++;
        }
    }

    results.time_taken = timePassed;
    results.mistakes = totalMistakes;
    results.keystrokes = totalKeystrokes;
    results.correct_chars = correct;
    results.total_chars = refLen;

    if (timePassed > 0) {
        results.wpm = (int)((correct / 5.0) / (timePassed / 60.0));
    }

    if (totalKeystrokes > 0) {
        results.accuracy = (correct * 100) / totalKeystrokes;
    }
        double waitStart = getCurrentTime();

    // Wait for 1 seconds or until user presses a key to show results
    while (getCurrentTime() - waitStart < 1.0) {
        if (isKeyPressed()) {
            readKey();  
        }
    }
    while (isKeyPressed()) {
        readKey();
    }

    displayTimeResults(results);
    waitForKey();

    restoreTerminal();
    clearScreen();

    return results;
}