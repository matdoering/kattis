#include <iostream>
#include <vector>
#include <string>

#define CHIKAPU 0

static size_t COUNTER = 0;

// returns true if a move was possible
bool modifyString(std::string& s) {
    // find allowed actions
    if (s.size() <= 2) {
        // cant modify 1st and last position
        return false;
    }
    // scan string to find action to take (greedily)
    for (size_t i = 1; i < s.size()-1; ++i) {
        if (s[i-1] != s[i+1]) {
            // move is allowed (no consecutive identical chars generated)
            s.erase(i, 1);
            return true;
        }
    }
    return false;
}

void trivialSolve(std::string& s) {
    size_t iterCount = 0;
    while (modifyString(s)) {
        //std::cout << samples[i] << std::endl;
        iterCount++;
    }
    if ((iterCount+1) % 2 == CHIKAPU) { // chikapu started playing
        std::cout << "Chikapu" << std::endl;
    } else {
        std::cout << "Bash" << std::endl;
    }
}

void smartSolve(std::string& s) {
    // the number of possible moves is fixed from the beginning
    // because every constraint can be resolved by a removal
    // except for the potentially last move (3 chars left)
    int N = s.size();
    int nbrAllowedMoves = N - 2;
    if (s[0] == s[N-1]) { 
        // cant remove the element in the middle of the first and last char
        nbrAllowedMoves -= 1;
    }
    if ((nbrAllowedMoves+1) % 2 == CHIKAPU) { // chikapu started playing
        std::cout << "Chikapu" << std::endl;
    } else {
        std::cout << "Bash" << std::endl;
    }
}

void interpretableSolve(std::string& s) {
    std::string winner;
    int N = s.size();
    bool isEvenString = (N % 2) == 0;
    if (isEvenString) {
        if (s[0] == s[N-1]) {
            // identical chars at beginning and end
            winner = "Chikapu";
        } else {
            winner = "Bash"; // bash always wins for even-length strings where first and last char is different
        }
    } else {
        if (s[0] == s[N-1]) {
            winner = "Bash"; // bash always wins for odd-length strings where first and last chars are the same
        } else {
            winner = "Chikapu";
        }
    }
    std::cout << winner << std::endl;
}

int main(int argc, char** argv) {
    // parse input
    int nbrSamples;
    std::cin >> nbrSamples;
    std::vector<std::string> samples;
    samples.reserve(nbrSamples);
    while (nbrSamples--) {
        std::string str;
        std::cin >> str;
        samples.push_back(str);
    }
    // solve
    // rule: cannot remove 1st and last char
    //       moves that make two consecutive chars identical are not allowed
    for (size_t i = 0; i< samples.size(); ++i) {
        //trivialSolve(samples[i]);
        //smartSolve(samples[i]);
        interpretableSolve(samples[i]);

    }
}

