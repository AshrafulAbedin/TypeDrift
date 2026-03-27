#include "TerminalSetup.h"
#include<iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

using namespace std;


struct termios originalSettings;

void setTerminal() {
    struct termios newSettings;
    tcgetattr(STDIN_FILENO, &originalSettings);
    newSettings = originalSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);
    newSettings.c_cc[VMIN] = 0;
    newSettings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);    
}

void restoreTerminal(){
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

std::string readPassword() {
    std::cout.flush();
    struct termios oldSettings, newSettings;
    tcgetattr(STDIN_FILENO, &oldSettings);
    newSettings = oldSettings;
    // Disable echo and canonical mode so we read char-by-char without display
    newSettings.c_lflag &= ~(ECHO | ICANON);
    newSettings.c_cc[VMIN] = 1;
    newSettings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
    std::string password;
    char ch;
    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == '\n' || ch == '\r') {
            // Enter pressed — done
            break;
        } else if (ch == 127 || ch == '\b') {
            // Backspace — remove last character and erase the '*'
            if (!password.empty()) {
                password.pop_back();
                // Move cursor back one, overwrite with space, move back again
                cout << "\b \b";
                cout.flush();
            }
        } else if (ch >= 32 && ch < 127) {
            // Printable ASCII character
            password += ch;
            cout << '*';
            cout.flush();
        }
    }
    cout << '\n';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
    return password;
}