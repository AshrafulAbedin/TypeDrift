#ifndef TerminalSetup_H
#define TerminalSetup_H

//ANSII COLOR CODES
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
#define BG_MAGNETA  "\033[45m"

//FUNCTION DECLARATIONS  
void setTerminal(); //set up terminal to take single characters as fast input
void restoreTerminal(); //restores default terminall settings
bool isKeyPressed(); //checks if a key is pressed
char readKey();  //reads pressed key
char waitForKey(); //waits for any key(blocks input)
void clearScreen(); //wipes the screen
double getCurrentTime(); //get time in seconds
int getLength(const char text[]); //geta a text's length

#endif