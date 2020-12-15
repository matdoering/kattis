#include "ILP.h"
#include <vector>
#include "/usr/lib/lpsolve/lp_lib.h"

ILPSolver::ILPSolver(std::vector<int> diceSequence) : m_diceSequence(diceSequence) {
    // build ILP
    // nbr of dice to be considered: 15 * 11 = 165
    // for ones, twos, ..., 6s: we need 165 vars each
    // chance: 165 vars
    // sequence: 6 * 165 vars
    // full house: 6 * 165 vars
    // four of a kind: 6 * 165 vars
    // five of a kind: 6 * 165 vars
    // total: 7 * 165 + 4 * (6 * 165)
    int nbrVars = 7 * 165 + 4 * (6 * 165);
    lprec* ilp = make_lp(0, nbrVars);

    // Variables are binary 
    for(int i = 1; i <= nbrVars; ++i)
    {
        set_binary(ilp, i, TRUE);
    }
     
    // add entries row by row
    set_add_rowmode(ilp, TRUE);
    
    // indices of the variables
    std::vector<int> colno(nbrVars);

    // coefficients of the variables
    std::vector<REAL> row(nbrVars, 1.0);

    int c = 9;
    for(int j = 1; j <= 8; ++j)
    {
        for(int i = 0; i < 8; ++i, ++c)
        {
            colno[i] = c;
        }
        if (!add_constraintex(ilp, 8, &row[0], &colno[0], EQ, 1.))
        {
            cerr << "Adding constraint failed" << endl;
            return false;
        }
    }
}

