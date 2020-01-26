#include "Solver.h"

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <chrono>

#define DEBUG 1

int main (int argc, char** argv) {
    auto t = std::chrono::steady_clock::now();
    std::string sampleFile;
    if (argc <= 1) {
        sampleFile = "";
        //std::cout << "No sample file provided, using std::in instead.";
    } else {
        sampleFile = argv[1];
    }
    Solver solver(sampleFile);
    auto answers = solver.solve();
    std::unordered_map<int, std::string> answer2string = {{0, "binary"}, {1, "decimal"}, {2, "neither"}};

    // write to file deactivated
    //std::ofstream fs;
    //fs.open(sampleFile + ".ans");
    for (Answer a : answers) {
        std::string answer = answer2string[(int)a];
        std::cout << answer << std::endl;
        //fs << answer + "\n";
    }
    //fs.close();
    auto tt = std::chrono::steady_clock::now();

    #if DEBUG
        std::cout << "Total | Elapsed time in milliseconds : " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(tt - t).count()
        << " ms" << std::endl;
    #endif
    return 0;
}
