#ifndef FW_ANIMATION_H
#define FW_ANIMATION_H

#include "fw_common.h"

// Falling word structure with blast/poof animation
struct FallingWord {
    char word[50];
    int wordLen;
    int row;
    int col;
    int matchedChars;
    int errorChars;
    bool active;
    
    // Animation state
    bool animating;
    int animFrame;
    long long animStartTime;
    static const int ANIM_DURATION = 600; // ms — longer for the blast effect
    static const int ANIM_FRAMES = 8;
    
    void init(const char* w, int startCol) {
        fw_strcpy(word, w);
        wordLen = fw_strlen(word);
        row = 1;
        col = startCol;
        matchedChars = 0;
        errorChars = 0;
        active = true;
        animating = false;
        animFrame = 0;
        animStartTime = 0;
    }
    
    void startAnimation(long long currentTime) {
        animating = true;
        animFrame = 0;
        animStartTime = currentTime;
    }
    
    bool updateAnimation(long long currentTime) {
        if(!animating) return false;
        long long elapsed = currentTime - animStartTime;
        animFrame = (int)((elapsed * ANIM_FRAMES) / ANIM_DURATION);
        if(elapsed >= ANIM_DURATION) {
            animating = false;
            return false;
        }
        return true;
    }
    
    void fall() {
        row++;
    }
    
    bool hasReachedBottom(int maxRow) {
        return row >= maxRow;
    }
};

// Manages and renders blast/poof animations for completed words
class AnimationManager {
public:
    static const int MAX_ANIMATIONS = 5;
    FallingWord animations[MAX_ANIMATIONS];
    
    AnimationManager() {
        for(int i = 0; i < MAX_ANIMATIONS; i++) {
            animations[i].animating = false;
        }
    }
    
    void startAnimation(FallingWord& completedWord, long long currentTime) {
        for(int i = 0; i < MAX_ANIMATIONS; i++) {
            if(!animations[i].animating) {
                fw_strcpy(animations[i].word, completedWord.word);
                animations[i].wordLen = completedWord.wordLen;
                animations[i].row = completedWord.row;
                animations[i].col = completedWord.col;
                animations[i].startAnimation(currentTime);
                return;
            }
        }
    }
    
    void update(long long currentTime) {
        for(int i = 0; i < MAX_ANIMATIONS; i++) {
            if(animations[i].animating) {
                animations[i].updateAnimation(currentTime);
            }
        }
    }
    
    void render(Terminal& term) {
        // Particle characters for the blast effect
        const char* particles[] = {"*", ".", "o", "'", "`", "~", "+", "^"};
        
        for(int i = 0; i < MAX_ANIMATIONS; i++) {
            if(!animations[i].animating) continue;
            
            int frame = animations[i].animFrame;
            int row = animations[i].row;
            int col = animations[i].col;
            int wlen = animations[i].wordLen;
            int centerCol = col + wlen / 2;
            
            if(frame < 2) {
                // Phase 1: Flash — word glows bright then white
                term.moveCursor(row, col);
                if(frame == 0) {
                    term.setColor(97); // Bright white
                } else {
                    term.setColor(93); // Bright yellow
                }
                term.print(animations[i].word);
                term.resetColor();
                
            } else if(frame < 4) {
                // Phase 2: Blast — word shatters outward with particles
                int spread = (frame - 1);
                
                // Scatter particles around the word position
                // Top particles
                if(row - spread > 0) {
                    term.moveCursor(row - spread, centerCol - 1);
                    term.setColor(93); // Yellow
                    term.print("\\");
                    term.print(" ");
                    term.print("/");
                    term.resetColor();
                }
                
                // Side particles
                term.moveCursor(row, col - spread);
                term.setColor(91); // Red
                term.print("<");
                term.resetColor();
                
                // Center — dissolving word
                term.moveCursor(row, col);
                term.setColor(33); // Dark yellow
                for(int j = 0; j < wlen; j++) {
                    int idx = (j + frame * 3) % 8;
                    term.print(particles[idx]);
                }
                term.resetColor();
                
                term.moveCursor(row, col + wlen + spread);
                term.setColor(91); // Red
                term.print(">");
                term.resetColor();
                
                // Bottom particles
                if(row + spread < 22) {
                    term.moveCursor(row + spread, centerCol - 1);
                    term.setColor(93); // Yellow
                    term.print("/");
                    term.print(" ");
                    term.print("\\");
                    term.resetColor();
                }
                
            } else if(frame < 6) {
                // Phase 3: Poof cloud — expanding smoke/cloud
                int spread = frame - 3;
                
                // Draw a cloud of particles
                for(int dr = -spread; dr <= spread; dr++) {
                    int drawRow = row + dr;
                    if(drawRow < 1 || drawRow > 22) continue;
                    
                    for(int dc = -spread - 1; dc <= spread + 1; dc++) {
                        int drawCol = centerCol + dc;
                        if(drawCol < 1 || drawCol > 79) continue;
                        
                        term.moveCursor(drawRow, drawCol);
                        // Outer ring is dim, inner is bright
                        int dist = (dr < 0 ? -dr : dr) + (dc < 0 ? -dc : dc);
                        if(dist <= 1) {
                            term.setColor(97); // Bright white — center
                            term.print("*");
                        } else if(dist <= spread) {
                            term.setColor(37); // White — middle
                            int idx = (drawRow + drawCol + frame) % 8;
                            term.print(particles[idx]);
                        } else {
                            term.setColor(90); // Dark gray — edge
                            term.print(".");
                        }
                        term.resetColor();
                    }
                }
                
            } else {
                // Phase 4: Score reveal — *poof* text + points
                term.moveCursor(row, centerCol - 4);
                term.setColor(95); // Bright magenta
                term.print("*poof*");
                term.resetColor();
                
                // Points display below
                if(row + 1 < 22) {
                    term.moveCursor(row + 1, centerCol - 2);
                    term.setColor(92); // Bright green
                    term.print("+");
                    
                    // Print the word length as points
                    char pts[8];
                    int pLen = 0;
                    int n = wlen;
                    if(n == 0) {
                        pts[pLen++] = '0';
                    } else {
                        char tmp[8];
                        int tLen = 0;
                        while(n > 0) {
                            tmp[tLen++] = '0' + (n % 10);
                            n /= 10;
                        }
                        for(int k = tLen - 1; k >= 0; k--) {
                            pts[pLen++] = tmp[k];
                        }
                    }
                    pts[pLen] = '\0';
                    term.print(pts);
                    term.print("pts");
                    term.resetColor();
                }
            }
        }
    }
};

#endif // FW_ANIMATION_H
