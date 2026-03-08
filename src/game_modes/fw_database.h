#ifndef FW_DATABASE_H
#define FW_DATABASE_H

#include "fw_common.h"

// Word database - loads words from file
class WordDatabase {
private:
    static const int MAX_WORDS = 500;
    static const int MAX_WORD_LEN = 50;
    static const int FILE_BUF_SIZE = 8192;
    static const int PATH_BUF_SIZE = 512;
    char wordStorage[MAX_WORDS][MAX_WORD_LEN];
    int wordCount;
    
    // Resolve the project root by reading /proc/self/exe
    // and stripping the binary name and build directory
    bool resolveProjectPath(char* outPath, int outSize, const char* relPath) {
        char exePath[PATH_BUF_SIZE];
        long len = syscall3(SYS_READLINK, (long)"/proc/self/exe", (long)exePath, PATH_BUF_SIZE - 1);
        if(len <= 0) return false;
        exePath[len] = '\0';
        
        // Strip the binary filename to get the directory
        int lastSlash = -1;
        for(int i = 0; i < len; i++) {
            if(exePath[i] == '/') lastSlash = i;
        }
        if(lastSlash < 0) return false;
        exePath[lastSlash + 1] = '\0';
        int dirLen = lastSlash + 1;
        
        // Check if directory ends with "build/" and strip it to get project root
        const char* buildSuffix = "build/";
        int buildLen = 6;
        if(dirLen >= buildLen) {
            bool isBuild = true;
            for(int i = 0; i < buildLen; i++) {
                if(exePath[dirLen - buildLen + i] != buildSuffix[i]) {
                    isBuild = false;
                    break;
                }
            }
            if(isBuild) {
                exePath[dirLen - buildLen] = '\0';
                dirLen -= buildLen;
            }
        }
        
        // Construct the full path: projectRoot + relPath
        int relLen = fw_strlen(relPath);
        if(dirLen + relLen >= outSize) return false;
        
        fw_memcpy(outPath, exePath, dirLen);
        fw_memcpy(outPath + dirLen, relPath, relLen);
        outPath[dirLen + relLen] = '\0';
        
        return true;
    }
    
public:
    WordDatabase() {
        wordCount = 0;
        
        // Try to resolve the path relative to the executable location
        char resolvedPath[PATH_BUF_SIZE];
        if(resolveProjectPath(resolvedPath, PATH_BUF_SIZE, "data/texts/falling_words.txt")) {
            loadFromFile(resolvedPath);
        }
        
        // Fallback: try relative paths if resolved path didn't work
        if(wordCount == 0) {
            loadFromFile("data/texts/falling_words.txt");
        }
        if(wordCount == 0) {
            loadFromFile("../data/texts/falling_words.txt");
        }
    }
    
    void loadFromFile(const char* path) {
        int fd = syscall3(SYS_OPEN, (long)path, 0, 0);
        if(fd < 0) return;
        
        char buf[FILE_BUF_SIZE];
        char currentWord[MAX_WORD_LEN];
        int currentWordLen = 0;
        
        long bytesRead;
        while((bytesRead = syscall3(SYS_READ, fd, (long)buf, FILE_BUF_SIZE)) > 0) {
            for(long i = 0; i < bytesRead; i++) {
                char c = buf[i];
                if(c == ' ' || c == '\n' || c == '\r' || c == '\t') {
                    if(currentWordLen > 0) {
                        currentWord[currentWordLen] = '\0';
                        addWord(currentWord);
                        currentWordLen = 0;
                    }
                } else {
                    if(currentWordLen < MAX_WORD_LEN - 1) {
                        currentWord[currentWordLen++] = c;
                    }
                }
            }
        }
        if(currentWordLen > 0) {
            currentWord[currentWordLen] = '\0';
            addWord(currentWord);
        }
        
        syscall1(SYS_CLOSE, fd);
    }
    
    void addWord(const char* word) {
        if(wordCount < MAX_WORDS) {
            fw_strcpy(wordStorage[wordCount], word);
            wordCount++;
        }
    }
    
    const char* getRandomWord() {
        if(wordCount == 0) return "word";
        return wordStorage[fw_rand() % wordCount];
    }
};

#endif // FW_DATABASE_H
