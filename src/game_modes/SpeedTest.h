#ifndef SPEEDTEST_H
#define SPEEDTEST_H

struct TestResults {
    int wpm;
    int accuracy;
    double time_taken;
    int mistakes;
    int keystrokes;
    int correct_chars;
    int total_chars;
};

TestResults runSpeedTest(bool saveToUser = false,int difficulty=1);

void displayResults(const TestResults& results);

#endif // SPEEDTEST_H