#ifndef FW_COMMON_H
#define FW_COMMON_H

// Syscall numbers for x86_64 Linux
#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_IOCTL 16
#define SYS_NANOSLEEP 35
#define SYS_CLOCK_GETTIME 228
#define SYS_READLINK 89

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
inline long syscall1(long n, long a1) {
    long ret;
    asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1) : "rcx", "r11", "memory");
    return ret;
}

inline long syscall2(long n, long a1, long a2) {
    long ret;
    asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2) : "rcx", "r11", "memory");
    return ret;
}

inline long syscall3(long n, long a1, long a2, long a3) {
    long ret;
    asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2), "d"(a3) : "rcx", "r11", "memory");
    return ret;
}

// Basic memory functions
inline void* fw_memset(void* s, int c, unsigned long n) {
    unsigned char* p = (unsigned char*)s;
    for(unsigned long i = 0; i < n; i++) {
        p[i] = (unsigned char)c;
    }
    return s;
}

inline void* fw_memcpy(void* dest, const void* src, unsigned long n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    for(unsigned long i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

inline int fw_strlen(const char* s) {
    int len = 0;
    while(s[len]) len++;
    return len;
}

inline int fw_strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

inline void fw_strcpy(char* dest, const char* src) {
    while(*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Random number generator (simple LCG)
static unsigned long randState = 12345;

inline void fw_srand(unsigned long seed) {
    randState = seed;
}

inline int fw_rand() {
    randState = randState * 1103515245 + 12345;
    return (randState / 65536) % 32768;
}

// Time functions
inline long long getCurrentTimeMs() {
    timespec ts;
    syscall2(SYS_CLOCK_GETTIME, 0, (long)&ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

inline void sleepMs(int ms) {
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
        syscall3(SYS_IOCTL, STDIN, TCGETS, (long)&originalSettings);
        
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
        syscall3(SYS_WRITE, STDOUT, (long)str, fw_strlen(str));
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

#endif // FW_COMMON_H
