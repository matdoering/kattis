#include <vector>

/* ILP solver for Yahtzee*/

/* Repetition of individual numbers:
 * 1s, 2s, 3s, 4s, 5s, 6s: 15 * 11 bits
 * constraint: use only 1s, 2s, ... etc
 * scoring function: sum of selected dice
 *
 * Chance: 
 * 15 * 11 bits
 * no special constraints
 * scoring function: sum
 *
 * All others: bit vectors for 1s, ... up to 6s with 15 * 11 bits each
 * idea: need to count the selected type of elements
 *
 * Sequence:
 * constraint is that no element is duplicated
 * and that the difference between sorted values is at most 1
 * simpler: only 1,2,3,4,5=15 or 2,3,4,5,6=20 is allowed and there is no duplication
 *
 * Full House:
 * two in some number seq selected, three in another number seq
 *
 * Four of a kind:
 * exact four in one number seq
 *
 * Five of a kind
 * exactly five in one number seq
 *
// enum class Combination {
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
*/

struct ILPSolver {
    ILPSolver(std::vector<int> diceSequence);
    std::vector<int> m_diceSequence;
};
