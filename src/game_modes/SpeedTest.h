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

// Main function - runs typing test and returns results
// Pass saveToUser = true when you want to save results to user profile
TestResults runSpeedTest(bool saveToUser = false);

// Display results in a nice format
void displayResults(const TestResults& results);

#endif // SPEEDTEST_H