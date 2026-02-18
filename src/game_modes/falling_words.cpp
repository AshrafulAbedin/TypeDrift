// Syscall numbers for x86_64 Linux
#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_IOCTL 16
#define SYS_NANOSLEEP 35
#define SYS_CLOCK_GETTIME 228

// IOCTL commands
#define TCGETS 0x5401
#define TCSETS 0x5402
#define TCSETSW 0x5403

// Termios flags
#define ICANON 0000002
#define ECHO 0000010
#define ISIG 0000001
#define VMIN 6
#define VTIME 5

// File descriptors
#define STDIN 0
#define STDOUT 1

// Structures for syscalls
struct termios {
    unsigned int c_iflag;
    unsigned int c_oflag;
    unsigned int c_cflag;
    unsigned int c_lflag;
    unsigned char c_line;
    unsigned char c_cc[32];
    unsigned int c_ispeed;
    unsigned int c_ospeed;
};

struct timespec {
    long tv_sec;
    long tv_nsec;
};

// Syscall wrappers
long syscall1(long n, long a1) {
    long ret;
    asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1) : "rcx", "r11", "memory");
    return ret;
}

long syscall2(long n, long a1, long a2) {
    long ret;
    asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2) : "rcx", "r11", "memory");
    return ret;
}

long syscall3(long n, long a1, long a2, long a3) {
    long ret;
    asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2), "d"(a3) : "rcx", "r11", "memory");
    return ret;
}

// Basic memory functions
void* memset(void* s, int c, unsigned long n) {
    unsigned char* p = (unsigned char*)s;
    for(unsigned long i = 0; i < n; i++) {
        p[i] = (unsigned char)c;
    }
    return s;
}

void* memcpy(void* dest, const void* src, unsigned long n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    for(unsigned long i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

int strlen(const char* s) {
    int len = 0;
    while(s[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void strcpy(char* dest, const char* src) {
    while(*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Random number generator (simple LCG)
unsigned long randState = 12345;

void srand(unsigned long seed) {
    randState = seed;
}

int rand() {
    randState = randState * 1103515245 + 12345;
    return (randState / 65536) % 32768;
}

// Time functions
long long getCurrentTimeMs() {
    timespec ts;
    syscall2(SYS_CLOCK_GETTIME, 0, (long)&ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

void sleepMs(int ms) {
    timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    syscall2(SYS_NANOSLEEP, (long)&ts, 0);
}

// Terminal control
class Terminal {
private:
    termios originalSettings;
    
public:
    void init() {
        // Get current settings
        syscall3(SYS_IOCTL, STDIN, TCGETS, (long)&originalSettings);
        
        // Set raw mode
        termios raw = originalSettings;
        raw.c_lflag &= ~(ICANON | ECHO | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        syscall3(SYS_IOCTL, STDIN, TCSETSW, (long)&raw);
        
        hideCursor();
        clearScreen();
    }
    
    void restore() {
        syscall3(SYS_IOCTL, STDIN, TCSETSW, (long)&originalSettings);
        showCursor();
        clearScreen();
    }
    
    void clearScreen() {
        const char* seq = "\033[2J\033[H";
        syscall3(SYS_WRITE, STDOUT, (long)seq, 10);
    }
    
    void moveCursor(int row, int col) {
        char buf[32];
        int len = 0;
        buf[len++] = '\033';
        buf[len++] = '[';
        
        if(row >= 100) buf[len++] = '0' + (row / 100);
        if(row >= 10) buf[len++] = '0' + ((row / 10) % 10);
        buf[len++] = '0' + (row % 10);
        buf[len++] = ';';
        
        if(col >= 100) buf[len++] = '0' + (col / 100);
        if(col >= 10) buf[len++] = '0' + ((col / 10) % 10);
        buf[len++] = '0' + (col % 10);
        buf[len++] = 'H';
        
        syscall3(SYS_WRITE, STDOUT, (long)buf, len);
    }
    
    void hideCursor() {
        const char* seq = "\033[?25l";
        syscall3(SYS_WRITE, STDOUT, (long)seq, 6);
    }
    
    void showCursor() {
        const char* seq = "\033[?25h";
        syscall3(SYS_WRITE, STDOUT, (long)seq, 6);
    }
    
    void print(const char* str) {
        syscall3(SYS_WRITE, STDOUT, (long)str, strlen(str));
    }
    
    void printAt(int row, int col, const char* str) {
        moveCursor(row, col);
        print(str);
    }
    
    void setColor(int colorCode) {
        char buf[16];
        int len = 0;
        buf[len++] = '\033';
        buf[len++] = '[';
        if(colorCode >= 10) buf[len++] = '0' + (colorCode / 10);
        buf[len++] = '0' + (colorCode % 10);
        buf[len++] = 'm';
        syscall3(SYS_WRITE, STDOUT, (long)buf, len);
    }
    
    void resetColor() {
        const char* seq = "\033[0m";
        syscall3(SYS_WRITE, STDOUT, (long)seq, 4);
    }
    
    char getChar() {
        char c;
        long n = syscall3(SYS_READ, STDIN, (long)&c, 1);
        return (n > 0) ? c : 0;
    }
};

// Word database
class WordDatabase {
private:
    static const int MAX_WORDS = 100;
    const char* words[MAX_WORDS];
    int wordCount;
    
public:
    WordDatabase() {
        wordCount = 0;
        addWord("hello");
        addWord("world");
        addWord("code");
        addWord("program");
        addWord("terminal");
        addWord("falling");
        addWord("tiles");
        addWord("game");
        addWord("type");
        addWord("speed");
        addWord("score");
        addWord("quick");
        addWord("jump");
        addWord("lazy");
        addWord("brown");
        addWord("fox");
        addWord("dog");
        addWord("cat");
        addWord("full");
        addWord("fulfilment");
        addWord("complete");
        addWord("system");
        addWord("linux");
        addWord("syscall");
        addWord("function");
        addWord("class");
        addWord("object");
        addWord("array");
        addWord("string");
        addWord("integer");
        addWord("float");
        addWord("double");
        addWord("char");
        addWord("void");
        addWord("return");
        addWord("break");
        addWord("continue");
        addWord("while");
        addWord("for");
        addWord("switch");
        addWord("case");
        addWord("default");
        addWord("struct");
        addWord("union");
        addWord("enum");
        addWord("typedef");
        addWord("const");
        addWord("static");
        addWord("extern");
        addWord("register");
        addWord("volatile");
    }
    
    void addWord(const char* word) {
        if(wordCount < MAX_WORDS) {
            words[wordCount++] = word;
        }
    }
    
    const char* getRandomWord() {
        if(wordCount == 0) return "word";
        return words[rand() % wordCount];
    }
};

// Falling word structure
struct FallingWord {
    char word[50];
    int wordLen;
    int row;
    int col;
    int matchedChars;
    bool active;
    
    void init(const char* w, int startCol) {
        strcpy(word, w);
        wordLen = strlen(word);
        row = 1;
        col = startCol;
        matchedChars = 0;
        active = true;
    }
    
    void fall() {
        row++;
    }
    
    bool hasReachedBottom(int maxRow) {
        return row >= maxRow;
    }
};

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
    }
    
    void spawnWord() {
        for(int i = 0; i < MAX_ACTIVE_WORDS; i++) {
            if(!words[i].active) {
                const char* word = wordDb.getRandomWord();
                int col = rand() % (SCREEN_WIDTH - strlen(word));
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
                    words[activeWordIndex].matchedChars = inputLen;
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
        
        // Try to match
        if(activeWordIndex == -1) {
            // Find best match (shortest word that matches)
            int bestMatch = -1;
            int shortestLen = 999;
            
            for(int i = 0; i < MAX_ACTIVE_WORDS; i++) {
                if(words[i].active && words[i].matchedChars == 0) {
                    bool matches = true;
                    for(int j = 0; j < inputLen; j++) {
                        if(words[i].word[j] != inputBuffer[j]) {
                            matches = false;
                            break;
                        }
                    }
                    
                    if(matches && words[i].wordLen < shortestLen) {
                        shortestLen = words[i].wordLen;
                        bestMatch = i;
                    }
                }
            }
            
            if(bestMatch != -1) {
                activeWordIndex = bestMatch;
                words[activeWordIndex].matchedChars = inputLen;
            } else {
                // No match, reset input
                inputLen = 0;
                inputBuffer[0] = '\0';
            }
        } else {
            // Continue matching active word
            FallingWord& active = words[activeWordIndex];
            
            if(active.word[inputLen - 1] == c) {
                active.matchedChars = inputLen;
                
                // Word completed?
                if(inputLen == active.wordLen) {
                    score += active.wordLen * 10;
                    active.active = false;
                    activeWordIndex = -1;
                    inputLen = 0;
                    inputBuffer[0] = '\0';
                }
            } else {
                // Mismatch
                active.matchedChars = 0;
                activeWordIndex = -1;
                inputLen = 0;
                inputBuffer[0] = '\0';
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
        printNumber(score);
        term.print("  Lives: ");
        printNumber(lives);
        
        // Draw falling words
        for(int i = 0; i < MAX_ACTIVE_WORDS; i++) {
            if(words[i].active) {
                term.moveCursor(words[i].row, words[i].col);
                
                if(i == activeWordIndex) {
                    term.setColor(32); // Green for active word
                    for(int j = 0; j < words[i].matchedChars; j++) {
                        char ch[2] = {words[i].word[j], '\0'};
                        term.print(ch);
                    }
                    term.setColor(33); // Yellow for remaining
                    term.print(words[i].word + words[i].matchedChars);
                    term.resetColor();
                } else {
                    term.print(words[i].word);
                }
            }
        }
        
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
        term.print("Type words to destroy them! ESC to quit");
    }
    
    bool isGameOver() {
        return lives <= 0;
    }
    
    int getScore() {
        return score;
    }
    
private:
    void printNumber(int n) {
        if(n == 0) {
            Terminal term;
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
        
        Terminal term;
        for(int i = len - 1; i >= 0; i--) {
            char ch[2] = {buf[i], '\0'};
            term.print(ch);
        }
    }
};

// Difficulty manager
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

// Main game loop
class Game {
private:
    Terminal terminal;
    GameState gameState;
    DifficultyManager difficulty;
    bool running;
    
public:
    Game() : running(true) {}
    
    void run() {
        terminal.init();
        
        // Seed random with current time
        srand(getCurrentTimeMs());
        
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
        
        int score = gameState.getScore();
        char scoreBuf[16];
        int len = 0;
        if(score == 0) {
            scoreBuf[len++] = '0';
        } else {
            int temp = score;
            while(temp > 0) {
                scoreBuf[len++] = '0' + (temp % 10);
                temp /= 10;
            }
            // Reverse
            for(int i = 0; i < len / 2; i++) {
                char t = scoreBuf[i];
                scoreBuf[i] = scoreBuf[len - 1 - i];
                scoreBuf[len - 1 - i] = t;
            }
        }
        scoreBuf[len] = '\0';
        terminal.print(scoreBuf);
        
        terminal.moveCursor(14, 25);
        terminal.print("Press any key to exit...");
        
        while(terminal.getChar() == 0) {
            sleepMs(50);
        }
        
        terminal.restore();
    }
};

// Entry point
int runFallingWords() {
    Game game;
    game.run();
    return 0;
}