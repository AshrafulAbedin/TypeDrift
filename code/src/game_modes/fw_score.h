#ifndef FW_SCORE_H
#define FW_SCORE_H

#include "fw_common.h"

// Utility to print numbers to the terminal (used for score/lives display)
inline void printNumber(Terminal& term, int n) {
    if(n == 0) {
        term.print("0");
        return;
    }
    
    char buf[16];
    int len = 0;
    bool negative = n < 0;
    if(negative) n = -n;
    
    while(n > 0) {
        buf[len++] = '0' + (n % 10);
        n /= 10;
    }
    
    if(negative) buf[len++] = '-';
    
    for(int i = len - 1; i >= 0; i--) {
        char ch[2] = {buf[i], '\0'};
        term.print(ch);
    }
}

// Formats an integer into a string buffer, returns the length
inline int formatNumber(char* out, int n) {
    int len = 0;
    if(n == 0) {
        out[len++] = '0';
        out[len] = '\0';
        return len;
    }
    
    bool negative = n < 0;
    if(negative) n = -n;
    
    char tmp[16];
    int tLen = 0;
    while(n > 0) {
        tmp[tLen++] = '0' + (n % 10);
        n /= 10;
    }
    if(negative) tmp[tLen++] = '-';
    
    for(int i = tLen - 1; i >= 0; i--) {
        out[len++] = tmp[i];
    }
    out[len] = '\0';
    return len;
}

// Difficulty manager — controls fall speed and spawn rate over time
class DifficultyManager {
private:
    long long startTime;
    int baseFallSpeed;
    int baseSpawnRate;
    
public:
    DifficultyManager() {
        startTime = getCurrentTimeMs();
        baseFallSpeed = 500;  // ms per fall
        baseSpawnRate = 2000; // ms between spawns
    }
    
    int getFallSpeed() {
        long long elapsed = getCurrentTimeMs() - startTime;
        int seconds = elapsed / 1000;
        
        int speedup = (seconds / 10) * 50;
        int speed = baseFallSpeed - speedup;
        
        return (speed < 100) ? 100 : speed;
    }
    
    int getSpawnRate() {
        long long elapsed = getCurrentTimeMs() - startTime;
        int seconds = elapsed / 1000;
        
        int speedup = (seconds / 15) * 200;
        int rate = baseSpawnRate - speedup;
        
        return (rate < 800) ? 800 : rate;
    }
};

#endif // FW_SCORE_H
