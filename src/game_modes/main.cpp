#include "TerminalSetup.h"
#include "SpeedTest.h"
#include <iostream>

int main(){
    std::cout<<"testing files\n";

    runSpeedTest();

    restoreTerminal();

    std::cout<<"....test finished...\n";
    return 0;
}