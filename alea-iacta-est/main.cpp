#include <iostream>
#include <cassert>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <unordered_map>
#include <numeric>
#include <unordered_set>
#include <iterator>
#include <map>
#include <sstream>
#include <algorithm>

#define DEBUG 1

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

int scoreRepetition(const std::vector<int>& rolls, int repeatedValue) {
    std::cout << repeatedValue << std::endl;
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

struct ScoreEntry {
    int score;
    bool wasScored;
};

// Model for evaluating the player dice rolls in Jahtzee
class Judge {
public:
    Judge() : m_totalScore(0), m_nbrOfRolls(0) { 
        initScoreSheet();
    }
    void registerRoll(const std::vector<int>& roll, Combination combi);
    int getTotalScore();
    void printScoreSheet();
    bool hasGameEnded() { return m_nbrOfRolls >= m_allowedNbrOfRolls; }
    
private:
    std::map<Combination, ScoreEntry> m_scoreSheet; // map from combination to score
    int m_totalScore;
    int m_nbrOfRolls;
    const int m_allowedNbrOfRolls = 11;
    void initScoreSheet();
};

void Judge::printScoreSheet() {
    int maxEntryChars = 28; // TODO hardcoded
    for (auto it = m_scoreSheet.begin(); it != m_scoreSheet.end(); ++it) {
        std::ostringstream os;
        os << it->first;
        std::string combi = os.str();
        int padding = maxEntryChars + 2 - combi.size();
        std::cout << combi << std::string(padding, ' ')
        << (it->second.wasScored ? "Scored!" : "NoScore") << " " << it->second.score << std::endl;;
    }
}

void Judge::initScoreSheet() {
    for (int i = 1; i <= 11; ++i) { // all combinations
        m_scoreSheet[static_cast<Combination>(i)] = {0, false};
    }
}

int Judge::getTotalScore() {
    // std::cout << "Score: " << m_totalScore << " at dice roll: " << m_nbrOfRolls << "/" << m_allowedNbrOfRolls << std::endl;
    return m_totalScore;
}

int scoreRoll(const std::vector<int>& rolls, Combination combi) {
    assert(rolls.size() == 5 && "Must use 5 dice per roll!");

    std::cout << "scoreRoll() for: " << combi << std::endl;
    printRoll(rolls);
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

void Judge::registerRoll(const std::vector<int>& roll, Combination combi) {
    assert(m_nbrOfRolls != m_allowedNbrOfRolls && "Too many rolls!");
    assert(m_scoreSheet.find(combi) != m_scoreSheet.end() && "cant register the same combination multiple times");

    int score = scoreRoll(roll, combi);
    m_scoreSheet[combi] = {score, true};
    m_totalScore += score;
    m_nbrOfRolls += 1;
}

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

struct Roll {
    Roll(const std::vector<int>& diceSequence) : m_curDiceIt(diceSequence.begin()) {
    }

    void roll(); /// performs a roll of five dice: updates m_diceValues and m_rolledDiceCount
    void reRoll(std::vector<int> reRollIdx); /// re-rolls the dice at the specified indices
    void registerRoll(Combination combi); /// registers the roll with the judge

    std::vector<int> m_diceValues; /// dice values of current five-dice roll
    std::vector<int>::const_iterator m_curDiceIt; // = m_diceSequence.begin(); // iterator to next dice value from RNG sequence
    int m_remainingReRolls = 2;
    // std::vector<int> m_diceSequence; /// known sequence of dices from RNG
    Judge m_judge;
};

void Roll::registerRoll(Combination combi) {
    m_judge.registerRoll(m_diceValues, combi);
    //m_judge.getTotalScore(); // just for cout
    m_judge.printScoreSheet();
    m_diceValues = {}; // reset dices so that player cant submit the same roll multiple times

    // ask judge whether game has ended
    if (m_judge.hasGameEnded()) {
        std::cout << "Game has ended with a total score of: " << m_judge.getTotalScore() << std::endl;
    }
}

void Roll::roll() {
    m_remainingReRolls = 2;
    auto oldDiceIt = m_curDiceIt;
    std::advance(m_curDiceIt, 5);
    m_diceValues = std::vector<int>(oldDiceIt,
                 m_curDiceIt);
    printRoll(m_diceValues);
}

void Roll::reRoll(std::vector<int> reRollIdx) {
    // TODO: for reRoll it is not 100% clear whether the reRoll idx can be assumed to be evaluated in
    // the order of the input or in the order of the indices by the user
    assert(m_remainingReRolls > 0 && "At most two re-rolls allowed");
    for (int i = 0; i < reRollIdx.size(); ++i) {
        int idx = reRollIdx[i];
        assert(idx >= 0 && idx < m_diceValues.size() && "Invalid reRoll idx!");
        // replace dice by another dice value (= reroll)
        m_diceValues[idx] = *m_curDiceIt;
        std::advance(m_curDiceIt, 1);
    }
    printRoll(m_diceValues);
    m_remainingReRolls -= 1;
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

void solveScenario(const Scenario& scenario) {
    RNG rng(scenario);
    auto diceSequence = determineDiceSequence(rng);
    Judge judge; // evaluates our selected rolls
    std::vector<int> roll = {1,1,1,1,1};

    Roll rollSequence(diceSequence);
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
    for (int i = 1; i <= 11; ++i) {
        rollSequence.roll();
        rollSequence.registerRoll(static_cast<Combination>(i));
    }
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
    std::cout << scoreSequence(testSeq) << std::endl;
    std::cout << scoreSequence(testSeq2) << std::endl;
    std::cout << scoreSequence(testSeq3) << std::endl;
    std::cout << scoreFullHouse(testFH) << std::endl;
    std::cout << scoreMultipleOfAKind(testMultiple, 5)<< std::endl;
    std::cout << scoreMultipleOfAKind(testMultiple2, 4)<< std::endl;
    std::cout << scoreChance(testMultiple2) << std::endl;
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
    test();
    return 0;
}
