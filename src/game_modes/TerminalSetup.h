#ifndef TerminalSetup_H
#define TerminalSetup_H

//ANSII COLOR CODES
#define CLEAR_SCREEN    "\033[2J\033[H"
#define RESET           "\033[0m"
#define RED             "\033[31m"
#define YELLOW          "\033[33m"
#define GRAY            "\033[90m"
#define BOLD            "\033[1m"
#define GREEN           "\033[32m"
#define CYAN            "\033[36m"
#define WHITE           "\033[97m"
#define BG_WHITE        "\033[47m"
#define BLACK           "\033[30m"
#define BG_MAGENTA      "\033[45m"
#define MAGNETA         "\033[35m"
#define BRIGHT_RED      "\033[91m"
#define BRIGHT_GREEN    "\033[92m"
#define BRIGHT_YELLOW   "\033[93m"
#define BRIGHT_BLUE     "\033[94m"
#define BRIGHT_MAGENTA  "\033[95m"
#define BRIGHT_CYAN     "\033[96m"
#define BRIGHT_WHITE    "\033[97m"
#define BG_BRIGHT_RED   "\033[101m"
#define BG_BRIGHT_GREEN "\033[102m"
#define BG_BRIGHT_YELLOW "\033[103m"
#define BG_BRIGHT_BLUE  "\033[104m"
#define BG_BRIGHT_MAGENTA "\033[105m"
#define BG_BRIGHT_CYAN  "\033[106m"
#define BG_BRIGHT_WHITE "\033[107m"
#define DIM             "\033[2m"
#define ITALIC          "\033[3m"
#define UNDERLINE       "\033[4m"
#define BLINK           "\033[5m"
#define REVERSE         "\033[7m"
#define HIDDEN          "\033[8m"
#define STRIKETHROUGH  "\033[9m"

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