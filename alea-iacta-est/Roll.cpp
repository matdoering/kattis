#include "Roll.h"
#include "Common.h"
#include "Scorer.h"

#include <cassert>
#include <iostream>

void Judge::printScoreSheet() const {
    printScoreCard(m_scoreSheet);
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

void Judge::registerRoll(const std::vector<int>& roll, Combination combi) {
    assert(m_nbrOfRolls != m_allowedNbrOfRolls && "Too many rolls!");
    auto it = m_scoreSheet.find(combi);
    assert(it != m_scoreSheet.end() && !it->second.wasScored && "cant register the same combination multiple times");

    int score = scoreRoll(roll, combi);
    it->second = {score, true};
    m_totalScore += score;
    m_nbrOfRolls += 1;
}


std::vector<int> Roll::registerRoll(Combination combi) {
    m_judge.registerRoll(m_diceValues, combi);
    //m_judge.getTotalScore(); // just for cout
    m_judge.printScoreSheet();
    m_diceValues = {}; // reset dices so that player cant submit the same roll multiple times

    // ask judge whether game has ended
    if (m_judge.hasGameEnded()) {
        std::cout << "Game has ended with a total score of: " << m_judge.getTotalScore() << std::endl;
        m_judge.printScoreSheet();
    }
    return m_diceValues;
}

void Roll::roll() {
    m_remainingReRolls = 2;
    auto oldDiceIt = m_curDiceIt;
    std::advance(m_curDiceIt, 5);
    m_diceValues = std::vector<int>(oldDiceIt,
                 m_curDiceIt);
    //printRoll(m_diceValues);
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
    //printRoll(m_diceValues);
    m_remainingReRolls -= 1;
}


