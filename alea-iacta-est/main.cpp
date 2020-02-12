#include "Common.h"
#include "Roll.h"
#include "Scorer.h"
#include "Genetic.h"

#include <iostream>
#include <cassert>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <numeric>
#include <map>
#include <sstream>
#include <algorithm>
#include <utility>

#define DEBUG 1



/// A game scenario defined by the initial random number generator state
struct Scenario {
    int A; // multiplier
    int C; // increment
    int64_t X; // initial seed
};

/// A linear congruential random number generator
class RNG {
public:
    RNG(const Scenario& s);
    int rollDice();
    
private:
    Scenario m_s;
    int64_t m_X; // current random number
};

RNG::RNG(const Scenario& s) :  m_s(s), m_X(m_s.X) {
    std::cout << "Playing scenario: " << s.A << " " << s.C << " " << s.X << std::endl;
}

int RNG::rollDice() {
    // update random sequence X
    // modulo: only select the lower 32 bits
    m_X = (m_s.A * m_X + m_s.C) % static_cast<int64_t>(pow(2, 32));
    // randomness is only in higher bits -> discard the 16 lower bits
    int roll = (static_cast<int>(m_X / pow(2, 16))) % 6 + 1;
    return roll;
}

/// determine sequence of dice from RNG
std::vector<int> determineDiceSequence(RNG& rng) {
    // determine sequence for longest possible game
    // 11 rounds with 6 dice. It's possible to re-roll 2 times
    // -> 11 * (6 * 3) rolls at most
    int maxNbrRolls = 11 * (6 * 3);
    std::vector<int> seq;
    seq.reserve(maxNbrRolls);
    while (maxNbrRolls--) {
        int roll = rng.rollDice();
        // output of RNG:
        // std::cout << "Roll: " << roll << std::endl;
        seq.push_back(roll);
    }
    return seq;
}

void solveArbitrarily(Roll& rollSequence) {
    for (int i = 1; i <= 11; ++i) {
        rollSequence.roll();
        rollSequence.registerRoll(static_cast<Combination>(i));
    }
}

void fillScoreSheetInInterval(std::map<Combination, ScoreEntry>& scoreSheet, 
                              const std::vector<int>& diceSequence, int i, int j,
                              const std::vector<int>& combinationsToConsider) {
    // evaluate all combinations in interval [i, j]
    for (int combiId : combinationsToConsider) {
        std::cout << "combi id: " << combiId << std::endl;
        Combination combi = static_cast<Combination>(combiId);
        int score = scoreRollInSeq(diceSequence, combi, i, j);
        if (score > scoreSheet[combi].score) {
            // std::cout << "Score for " << combi << ": "  << score << std::endl;;
            scoreSheet[combi].score = score;
            scoreSheet[combi].idx = std::make_pair(i,j);
        }
    }
    std::cout << "new:" << std::endl;
    printScoreCard(scoreSheet);
}

std::map<Combination, ScoreEntry> createScoreSheet(const std::vector<int>& diceSequence) {
    std::map<Combination, ScoreEntry> scoreSheet;

    std::vector<int> combinationsToConsider(11);
    std::iota(combinationsToConsider.begin(), combinationsToConsider.end(), 1); // seq from 1 to 11
    for (int i = 0; i < diceSequence.size() -5; ++i) {
        int j = i + 5; // closed interval
        fillScoreSheetInInterval(scoreSheet, diceSequence, i, j, combinationsToConsider);
    }
    return scoreSheet;
}

void solveGreedily(const std::vector<int>& diceSequence) {
    // idea: find combinations in the sequence that give the largest score and try to reach these combinations
    // e.g. if there is a 'five of a kind' late in the RNG, then try to move further to reach these points
    // so .... we just scan the sequence in intervals from 0 to 4, 1 to 5, 2 to 6, .., to find the combination
    // that maximizes the score for the interval ... later on: finetuning by rerolling etc

    auto scoreSheet = createScoreSheet(diceSequence);
    printScoreCard(scoreSheet);
    Roll rollSequence(diceSequence);
}

void solveVeryGreedily(const std::vector<int>& diceSequence) {
    Roll rollSequence(diceSequence);
    std::vector<int> combinationsToConsider(11);
    std::iota(combinationsToConsider.begin(), combinationsToConsider.end(), 1); // seq from 1 to 11
    int i = 0;
    
    for (int nbrPlays = 0; nbrPlays < 11; nbrPlays++) { // without re-rolls
        std::map<Combination, ScoreEntry> scoreSheet;
        int j = i+5;
        fillScoreSheetInInterval(scoreSheet, diceSequence, i, j, combinationsToConsider);
        // find the maximum entry (TODO: slow)
        int maxScore = 0;
        Combination combi;
        for (auto it = scoreSheet.begin(); it != scoreSheet.end(); ++it) {
            if (it->second.score >= maxScore) {
                combi = it->first;
                maxScore = it->second.score;
            }
        }
        std::cout << "PLAYING: " << combi << std::endl;
        // just play the combination maximizing the interval value without re-rolling the dice
        rollSequence.roll();
        rollSequence.registerRoll(combi);
        i += 5;
        // remove combination from combis to consider
        combinationsToConsider.erase(std::find(combinationsToConsider.begin(), combinationsToConsider.end(), static_cast<int>(combi)));
    }

}


void solveScenario(const Scenario& scenario) {
    RNG rng(scenario);
    auto diceSequence = determineDiceSequence(rng);
    Roll rollSequence(diceSequence);
    //solveArbitrarily(rollSequence);
    solveGreedily(diceSequence);
    GParams params = {50, 0.01};
    solveGenetic(diceSequence, params);
    //solveVeryGreedily(diceSequence); // score: 49
    /*
    rollSequence.roll();
    rollSequence.reRoll({4,0});
    rollSequence.reRoll({0,1,2,3,4});
    rollSequence.registerRoll(Combination::SIXES);
    rollSequence.roll();
    rollSequence.registerRoll(Combination::FIVES);
    rollSequence.roll();
    rollSequence.registerRoll(Combination::FULL_HOUSE);
    */
}

void test() {
    // test some pow stuff
    int x = static_cast<int>(pow(2,32)); // 2147483647
    long int y = static_cast<int64_t>(pow(2,32)); // long int: 4294967296
    std::cout << y << std::endl;
    // test rolls
    std::vector<int> test = {1,3,5,1,4};
    std::vector<int> testSeq = {1,2,3,4,5};
    std::vector<int> testSeq2 = {3,4,5,2,1};
    std::vector<int> testSeq3 = {3,3,4,5,2};
    std::vector<int> testFH = {1,3,1,3,1};
    std::vector<int> testMultiple = {1,1,1,1,1};
    std::vector<int> testMultiple2 = {1,1,1,1,5};
    scoreRoll(test, Combination::ONES);
    std::cout << scoreRoll(testSeq, Combination::SEQUENCE) << std::endl;
    std::cout << scoreRoll(testSeq2, Combination::SEQUENCE) << std::endl;
    std::cout << scoreRoll(testSeq3, Combination::SEQUENCE) << std::endl;
    std::cout << scoreRoll(testFH, Combination::FULL_HOUSE) << std::endl;
    std::cout << scoreRoll(testMultiple, Combination::ONES)<< std::endl;
    std::cout << scoreRoll(testMultiple2, Combination::ONES)<< std::endl;
    std::cout << scoreRoll(testMultiple2, Combination::CHANCE) << std::endl;
}


int main(int argc, char** argv) {
    // input defines linear congruential generator: 
    // X_n+1 = (A X_n + C) mod 2^32
    int A = 1;
    int C = 1;
    int X = 1;
    while (A || C || X) {
        std::cin >> A >> C >> X;
        Scenario s{A, C, X};
        solveScenario(s);
        break; // only solve one scenario, TODO
    }
    //test();
    return 0;
}
