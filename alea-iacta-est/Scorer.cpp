#include "Scorer.h"

#include <unordered_map>
#include <numeric>
#include <iostream>
#include <cassert>
#include <algorithm>

int scoreRepetition(const std::vector<int>& rolls, int repeatedValue) {
    int score = 0;
    for (const auto& roll : rolls) {
        if (roll == repeatedValue) {
            score += repeatedValue;
        }
    }
    return score;
}

int scoreSequence(const std::vector<int>& rolls) {
    int score = 0;
    std::vector<int> orderedVec(rolls.begin(), rolls.end());
    std::sort(orderedVec.begin(), orderedVec.end());
    for (int i = 0; i < orderedVec.size(); ++i) {
        if (i != orderedVec.size() - 1) {
            if (orderedVec[i+1] - orderedVec[i] != 1) {
                // not a sequence
                score = 0;
                break;
            }
        } 
        score += orderedVec[i];
    }
    return score;
}

int scoreFullHouse(const std::vector<int>& rolls) {
    // need to ensure that one value is repeated 3 times, the other one two times
    std::unordered_map<int, int> rollCounts; // how often each number was rolled
    for (int roll : rolls) {
        rollCounts[roll] += 1;
    }
    if (rollCounts.size() != 2) {
        // no full house possible
        return 0;
    }
    // check full house
    int score = 0;
    for (auto it = rollCounts.begin(); it != rollCounts.end(); ++it) {
        if (it->second == 3 || it->second == 2) {
            score += it->first*it->second;
        } else {
            score = 0;
            break;
        }
    }
    return score;
}

int scoreChance(const std::vector<int>& rolls) {
    return std::accumulate(rolls.begin(), rolls.end(), 0);
}
int scoreMultipleOfAKind(const std::vector<int>& rolls, int N) {
    assert(N >= 4 && "Need N >= 4 for scoreMultipleOfAKind");

    std::unordered_map<int, int> rollCounts; // how often each number was rolled
    for (int roll : rolls) {
        rollCounts[roll] += 1;
    }
    // check 'N' of a kind
    bool foundN = false;
    int score = 0;
    for (auto it = rollCounts.begin(); it != rollCounts.end(); ++it) {
        if (it->second == N) {
            foundN = true;
        }
        score += it->first*it->second;
    }
    if (foundN && N == 5) {
        return 50; // special rule
    } else if (foundN) {
        return score; // sum of values
    } else {
        return 0; // condition not fulfilled
    }
}

int scoreRoll(const std::vector<int>& rolls, Combination combi) {
    assert(rolls.size() == 5 && "Must use 5 dice per roll!");

    //std::cout << "scoreRoll() for: " << combi << std::endl;
    //printRoll(rolls);
    switch (combi) {
        case Combination::ONES:
        case Combination::TWOS:
        case Combination::THREES:
        case Combination::FOURS:
        case Combination::FIVES:
        case Combination::SIXES:
            return scoreRepetition(rolls, static_cast<int>(combi));
        case Combination::SEQUENCE:
            return scoreSequence(rolls);
        case Combination::FULL_HOUSE:
            return scoreFullHouse(rolls);
        case Combination::FOUR_OF_A_KIND:
            return scoreMultipleOfAKind(rolls, 4);
        case Combination::FIVE_OF_A_KIND:
            return scoreMultipleOfAKind(rolls, 5);
        case Combination::CHANCE:
            return scoreChance(rolls);
        default:
            std::abort(); 
    }
}

int scoreRollInSeq(const std::vector<int>& diceValues, Combination combi, int start, int end) {
    std::vector<int> subSeq = std::vector<int>(diceValues.begin() + start, diceValues.begin() + end); // TODO: this is not very fast (vector copy is unnecessary if we work on contiguous dice data)
    return scoreRoll(subSeq, combi);
}


