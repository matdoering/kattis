#pragma once

#include <vector>
#include "Common.h"

// Model for evaluating the player dice rolls in Jahtzee
class Judge {
public:
    Judge() : m_totalScore(0), m_nbrOfRolls(0) { 
        initScoreSheet();
    }
    void registerRoll(const std::vector<int>& roll, Combination combi);
    int getTotalScore();
    void printScoreSheet() const;
    bool hasGameEnded() { return m_nbrOfRolls >= m_allowedNbrOfRolls; }
    
private:
    std::map<Combination, ScoreEntry> m_scoreSheet; // map from combination to score
    int m_totalScore;
    int m_nbrOfRolls;
    const int m_allowedNbrOfRolls = 11;
    void initScoreSheet();
};

struct Roll {
    Roll(const std::vector<int>& diceSequence) : m_curDiceIt(diceSequence.begin()) {
    }

    void roll(); /// performs a roll of five dice: updates m_diceValues and m_rolledDiceCount
    void reRoll(std::vector<int> reRollIdx); /// re-rolls the dice at the specified indices
    std::vector<int> registerRoll(Combination combi); /// registers the roll with the judge, returns the roll

    std::vector<int> m_diceValues; /// dice values of current five-dice roll
    std::vector<int>::const_iterator m_curDiceIt; // = m_diceSequence.begin(); // iterator to next dice value from RNG sequence
    int m_remainingReRolls = 2;
    // std::vector<int> m_diceSequence; /// known sequence of dices from RNG
    Judge m_judge;
};


