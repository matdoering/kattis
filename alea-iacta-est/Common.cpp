#include "Common.h"

#include <string>
#include <iostream>
#include <map>
#include <vector>

std::ostream& operator<<(std::ostream& os, Combination c) {
    #define ENTRY(name) case name: s = #name; break;
    std::string s;
    switch(c) {
       ENTRY(Combination::ONES)
       ENTRY(Combination::TWOS)
       ENTRY(Combination::THREES)
       ENTRY(Combination::FOURS)
       ENTRY(Combination::FIVES)
       ENTRY(Combination::SIXES)
       ENTRY(Combination::SEQUENCE)
       ENTRY(Combination::FULL_HOUSE)
       ENTRY(Combination::FOUR_OF_A_KIND)
       ENTRY(Combination::FIVE_OF_A_KIND)
       ENTRY(Combination::CHANCE)
       default:
            s = "UNKNOWN";
            break;
    }
    #undef ENTRY
    os << s;
    return os;
}
void printRoll(const std::vector<int>& rolls) {
    std::cout << "Roll: ";
    for (int i = 0; i < rolls.size(); ++i) {
        std::cout << rolls[i] << " ";
    }
    std::cout << std::endl;
}


void printScoreCard(const std::map<Combination, ScoreEntry>& scoreSheet) {
    int maxEntryChars = 28; // TODO hardcoded
    for (auto it = scoreSheet.begin(); it != scoreSheet.end(); ++it) {
        std::ostringstream os;
        os << it->first;
        std::string combi = os.str();
        int padding = maxEntryChars + 2 - combi.size();
        std::cout << combi << std::string(padding, ' ')
        << (it->second.wasScored ? "Scored!" : "NoScore") << " " << it->second.score
        << " (" << it->second.idx.first << "," << it->second.idx.second << ")"
        << std::endl;;
    }
}

