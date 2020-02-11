#pragma once

#include <sstream>
#include <map>
#include <vector>

enum class Combination {
    ONES = 1, 
    TWOS,
    THREES,
    FOURS,
    FIVES,
    SIXES,
    SEQUENCE,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    FIVE_OF_A_KIND,
    CHANCE
};

std::ostream& operator<<(std::ostream& os, Combination c);

void printRoll(const std::vector<int>& rolls);

struct ScoreEntry {
    int score;
    bool wasScored;
    std::pair<int,int> idx; // idx to find in RNG list of dices
};

void printScoreCard(const std::map<Combination, ScoreEntry>& scoreSheet);

