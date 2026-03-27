#include "fw_common.h"
#include "fw_animation.h"
#include "fw_database.h"
#include "fw_score.h"

// Game state manager
class GameState {
private:
    static const int MAX_ACTIVE_WORDS = 10;
    static const int SCREEN_WIDTH = 80;
    static const int SCREEN_HEIGHT = 24;
    static const int INPUT_ROW = 23;
    
    FallingWord words[MAX_ACTIVE_WORDS];
    int activeWordIndex;
    char inputBuffer[50];
    int inputLen;
    int score;
    int lives;
    long long lastSpawnTime;
    long long lastFallTime;
    WordDatabase wordDb;
    AnimationManager animManager;
    
public:
    GameState() {
        activeWordIndex = -1;
        inputLen = 0;
        score = 0;
        lives = 3;
        lastSpawnTime = 0;
        lastFallTime = 0;
        
        for(int i = 0; i < MAX_ACTIVE_WORDS; i++) {
            words[i].active = false;
        }
        inputBuffer[0] = '\0';
    }
    
    // Find the index of the lowest active word (highest row value)
    int findLowestWord() {
        int lowestIdx = -1;
        int maxRow = -1;
        for(int i = 0; i < MAX_ACTIVE_WORDS; i++) {
            if(words[i].active && words[i].row > maxRow) {
                maxRow = words[i].row;
                lowestIdx = i;
            }
        }
        return lowestIdx;
    }
    
    void update(long long currentTime, int fallSpeed, int spawnRate) {
        // Make words fall
        if(currentTime - lastFallTime >= fallSpeed) {
            lastFallTime = currentTime;
            
            for(int i = 0; i < MAX_ACTIVE_WORDS; i++) {
                if(words[i].active) {
                    words[i].fall();
                    
                    if(words[i].hasReachedBottom(INPUT_ROW)) {
                        words[i].active = false;
                        lives--;
                        if(activeWordIndex == i) {
                            activeWordIndex = -1;
                            inputLen = 0;
                            inputBuffer[0] = '\0';
                        }
                    }
                }
            }
        }
        
        // Spawn new words
        if(currentTime - lastSpawnTime >= spawnRate) {
            lastSpawnTime = currentTime;
            spawnWord();
        }
        
        // Update animations
        animManager.update(currentTime);
    }
    
    void spawnWord() {
        for(int i = 0; i < MAX_ACTIVE_WORDS; i++) {
            if(!words[i].active) {
                const char* word = wordDb.getRandomWord();
                int wordLen = fw_strlen(word);
                int maxCol = SCREEN_WIDTH - wordLen - 2;
                if(maxCol < 1) maxCol = 1;
                int col = (fw_rand() % maxCol) + 1;
                words[i].init(word, col);
                return;
            }
        }
    }
    
    bool processInput(char c) {
        if(c == 27) { // ESC
            return false;
        }
        
        if(c == 127 || c == 8) { // Backspace
            if(inputLen > 0) {
                inputLen--;
                inputBuffer[inputLen] = '\0';
                
                if(activeWordIndex != -1) {
                    FallingWord& active = words[activeWordIndex];
                    if(active.errorChars > 0) {
                        active.errorChars--;
                    } else if(active.matchedChars > 0) {
                        active.matchedChars--;
                    }
                    if(active.matchedChars == 0 && active.errorChars == 0) {
                        activeWordIndex = -1;
                    }
                }
            }
            return true;
        }
        
        if(c < 32 || c > 126) {
            return true;
        }
        
        // Add to input buffer
        if(inputLen < 49) {
            inputBuffer[inputLen++] = c;
            inputBuffer[inputLen] = '\0';
        }
        
        // Try to match — must type the LOWEST word first
        if(activeWordIndex == -1) {
            int lowestIdx = findLowestWord();
            
            if(lowestIdx != -1 && words[lowestIdx].matchedChars == 0) {
                if(words[lowestIdx].word[0] == c) {
                    activeWordIndex = lowestIdx;
                    words[activeWordIndex].matchedChars = 1;
                    words[activeWordIndex].errorChars = 0;
                } else {
                    inputLen = 0;
                    inputBuffer[0] = '\0';
                }
            } else {
                inputLen = 0;
                inputBuffer[0] = '\0';
            }
        } else {
            FallingWord& active = words[activeWordIndex];
            
            if(active.errorChars > 0) {
                active.errorChars++;
            } else if(active.word[active.matchedChars] == c) {
                active.matchedChars++;
                
                // Word completed?
                if(active.matchedChars == active.wordLen) {
                    score += active.wordLen;
                    animManager.startAnimation(active, getCurrentTimeMs());
                    active.active = false;
                    activeWordIndex = -1;
                    inputLen = 0;
                    inputBuffer[0] = '\0';
                }
            } else {
                active.errorChars = 1;
            }
        }
        
        return true;
    }
    
    void render(Terminal& term) {
        term.clearScreen();
        
        // Draw border
        term.moveCursor(0, 0);
        for(int i = 0; i < SCREEN_WIDTH; i++) {
            term.print("=");
        }
        
        // Draw score and lives
        term.moveCursor(0, 2);
        term.print("Score: ");
        printNumber(term, score);
        term.print("  Lives: ");
        printNumber(term, lives);
        
        // Find the lowest word to highlight it
        int lowestIdx = findLowestWord();
        
        // Draw falling words
        for(int i = 0; i < MAX_ACTIVE_WORDS; i++) {
            if(words[i].active) {
                term.moveCursor(words[i].row, words[i].col);
                
                if(i == activeWordIndex) {
                    // Green for correctly matched chars
                    term.setColor(32);
                    for(int j = 0; j < words[i].matchedChars; j++) {
                        char ch[2] = {words[i].word[j], '\0'};
                        term.print(ch);
                    }
                    // Red for error chars
                    if(words[i].errorChars > 0) {
                        term.setColor(91);
                        int errEnd = words[i].matchedChars + words[i].errorChars;
                        if(errEnd > words[i].wordLen) errEnd = words[i].wordLen;
                        for(int j = words[i].matchedChars; j < errEnd; j++) {
                            char ch[2] = {words[i].word[j], '\0'};
                            term.print(ch);
                        }
                        term.setColor(37);
                        if(errEnd < words[i].wordLen) {
                            term.print(words[i].word + errEnd);
                        }
                    } else {
                        term.setColor(37);
                        term.print(words[i].word + words[i].matchedChars);
                    }
                    term.resetColor();
                } else if(i == lowestIdx) {
                    term.setColor(93); // Bright yellow — type this one!
                    term.print(words[i].word);
                    term.resetColor();
                } else {
                    term.setColor(37); // White for other words
                    term.print(words[i].word);
                    term.resetColor();
                }
            }
        }
        
        // Draw blast/poof animations
        animManager.render(term);
        
        // Draw input area
        term.moveCursor(INPUT_ROW, 0);
        for(int i = 0; i < SCREEN_WIDTH; i++) {
            term.print("=");
        }
        
        term.moveCursor(INPUT_ROW + 1, 0);
        term.print("> ");
        term.print(inputBuffer);
        
        // Instructions
        term.moveCursor(SCREEN_HEIGHT - 1, 0);
        term.print("Type the LOWEST word first! ESC to quit");
    }
    
    bool isGameOver() {
        return lives <= 0;
    }
    
    int getScore() {
        return score;
    }
};

// Main game loop
class Game {
private:
    Terminal terminal;
    GameState gameState;
    DifficultyManager difficulty;
    bool running;
    
public:
    Game() : running(true) {}
    
    int run() {
        terminal.init();
        
        // Seed random with current time
        fw_srand(getCurrentTimeMs());
        
        long long lastUpdate = getCurrentTimeMs();
        
        while(running) {
            long long currentTime = getCurrentTimeMs();
            
            // Update game state
            int fallSpeed = difficulty.getFallSpeed();
            int spawnRate = difficulty.getSpawnRate();
            gameState.update(currentTime, fallSpeed, spawnRate);
            
            // Process input
            char c = terminal.getChar();
            if(c != 0) {
                if(!gameState.processInput(c)) {
                    running = false;
                }
            }
            
            // Render
            if(currentTime - lastUpdate >= 16) { // ~60 FPS
                lastUpdate = currentTime;
                gameState.render(terminal);
                
                if(gameState.isGameOver()) {
                    running = false;
                }
            }
            
            sleepMs(10);
        }
        
        // Game over screen
        terminal.clearScreen();
        terminal.moveCursor(10, 30);
        terminal.print("GAME OVER!");
        terminal.moveCursor(12, 28);
        terminal.print("Final Score: ");
        
        char scoreBuf[16];
        formatNumber(scoreBuf, gameState.getScore());
        terminal.print(scoreBuf);
        
        terminal.moveCursor(14, 25);
        terminal.print("Press any key to exit...");
        
        while(terminal.getChar() == 0) {
            sleepMs(50);
        }
        
        terminal.restore();
        return gameState.getScore();
    }
};

// Entry point
int runFallingWords() {
    Game game;
    return game.run();
}