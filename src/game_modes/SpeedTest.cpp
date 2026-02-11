#include "SpeedTest.h"
#include "TerminalSetup.h"
#include <iostream>
#include <../utils/file_helper.h>

using namespace std;

// Helper function to display the typing screen
//is called in every 50ms....so 20 times in 1 second!!! :3
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
                cout << BRIGHT_RED << BOLD << reference[i] << RESET;
            }
        } else {
            cout << RED << reference[i] << RESET;
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
    int percent=(userLen*100)/refLen;
    cout<<"Progress: "<< percent<<" % "<<endl;
    
    cout.flush();
}


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
        cout << GREEN << "  Excellent! You can say I am speed >_<!" << RESET << endl;
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

TestResults runSpeedTest(bool saveToUser) {

    
    setTerminal();
    
    TestResults results = {0, 0, 0.0, 0, 0, 0, 0};

        /*geting the current time and doing a mod 10 to get values 0-9
    then creating a memory location to append the number
    adding the asci value with the file number to get the string of the digit*/
    long current_time=(long)getCurrentTime();
    std::cout << "Trying to open " << std::endl;
    int fileindex=current_time % 10;
    char filename[50];
    const char base_text[] ="../data/texts/practice";
    int i=0;
    while(base_text[i] !='\0'){
        filename[i]=base_text[i];
        i++;
    }
    filename[i++] ='0' + fileindex;
    filename[i++] = '.';
    filename[i++] = 't';
    filename[i++] = 'x';
    filename[i++] = 't';
    filename[i]='\0';

    // Load reference text
    string text = FileHandler::readFile(filename);
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
            results.wpm = (int)((correct / 5.0) / (timePassed / 60.0));
        }
        
        // Calculate accuracy
        if (refLen > 0) {
            results.accuracy = (correct * 100) / refLen;
        }
        
        displayResults(results);
        
        // Wait for user choice
        waitForKey();
        
    
    
    restoreTerminal();
    clearScreen();
    
    return results;
}