#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>

using namespace std;

void createDirectory(const string& path) {
    #ifdef _WIN32
        // Windows
        string command = "mkdir \"" + path + "\" 2>nul";
    #else
        // Linux / Mac
        string command = "mkdir -p \"" + path + "\"";
    #endif
    system(command.c_str());
}

int main() {
    string modes[] = {"easy", "medium", "hard"};
    int filesPerMode = 50;
    string outputDir = "texts";

    createDirectory(outputDir);

    int totalFiles = 0;

    for (const string& mode : modes) {
        string modeDir = outputDir + "/" + mode;
        createDirectory(modeDir);

        for (int i = 1; i <= filesPerMode; i++) {
            string filename = modeDir + "/" + mode + to_string(i) + ".txt";

            ofstream file(filename);
            if (file.is_open()) {
                file.close();  // creates an empty file
                cout << "Created: " << filename << endl;
                totalFiles++;
            } else {
                cout << "ERROR: Could not create " << filename << endl;
            }
        }
    }

    cout << "\nDone! Generated " << totalFiles << " text files." << endl;

    return 0;
}