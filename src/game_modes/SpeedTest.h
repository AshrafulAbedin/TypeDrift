#ifndef SPEEDTEST_H
#define SPEEDTEST_H

struct TestResults {
    int wpm;
    int accuracy;
    double time_taken;
    int mistakes;
    int keystrokes;
};

void runSpeedTest();
TestResults runSpeedTestWithResults();  // New function

#endif // SPEEDTEST_H