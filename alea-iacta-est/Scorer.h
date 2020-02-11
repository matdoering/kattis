#pragma <once>

#include "Common.h"

#include <vector>

/*
int scoreRepetition(const std::vector<int>& rolls, int repeatedValue);
int scoreSequence(const std::vector<int>& rolls);
int scoreFullHouse(const std::vector<int>& rolls);
int scoreChance(const std::vector<int>& rolls);
int scoreMultipleOfAKind(const std::vector<int>& rolls, int N);
*/
int scoreRoll(const std::vector<int>& rolls, Combination combi);




int scoreRollInSeq(const std::vector<int>& diceValues, Combination combi, int start, int end);


