#include "Genetic.h"
#include "Common.h"
#include "Roll.h"
#include "Scorer.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <cassert>

// for recombination: only recombine "sets of five genes" that make up a dice roll so as to
// ensure that jahtzee constraints hold
// only recombine in the area where both values have something other than 0s (left bit vector is mostly 0)
// TODO: for recombination: may / should change data structure of chromosome
// -> need to have a function to select the blocks (5 grouping) ...
// blocks are dynamic so this is just a getter: getblock(int blockNr) -> interval (i,j)

namespace {

double randProb() {
    return ((double)rand()/RAND_MAX);
}
}

std::ostream& operator<<(std::ostream& os, const Chromosome& c) {
    os << c.chrom << "\n";
    int cLen = c.chrom.size();
    int shift = cLen - c.intervals.at(11).second - 1;
    os << std::string(shift, ' ');
    for (int i = 11; i >= 1; --i) {
        char symbol = '-';
        if (i % 2) {
            // chars to indicate differing combinations in the sequence
           symbol = '+'; 
        }
        const auto& interval = c.intervals.at(i);
        //std::cout << "interval for group " << i << ":" << interval.first << ", " << interval.second << std::endl;
        int shift = interval.second - interval.first + 1;
        os << std::string(shift, symbol);
    }
    return os;
}

void Chromosome::shiftLeft(int geneGroup, int by) {
    if (geneGroup == 11) {
        // no need to shift or correct intervals for following gene group
        return;
    }
    // update chrom representation after geneGroup
    auto oldChrom = chrom;
    auto& pos = intervals.at(geneGroup+1);
    for (int oldPos = pos.first; oldPos+by < chrom.size(); ++oldPos) {
        chrom.set(oldPos+by, oldChrom.test(oldPos));
    }
}

void Chromosome::shiftRight(int geneGroup, int by) {
    assert(by < 0 && "shift right should be negative value");
    if (geneGroup == 11) {
        // no need to shift or correct intervals for following gene groups because there are none
        return;
    }
    // update chrom representation after geneGroup
    auto oldChrom = chrom;
    auto& pos = intervals.at(geneGroup+1);
    for (int oldPos = pos.first; oldPos < chrom.size(); ++oldPos) {
        chrom.set(oldPos+by, oldChrom.test(oldPos));
    }
}

void Chromosome::replaceGeneGroup(int geneGroup, const Chromosome& other) {
    // find location in p1 
    auto p1int = other.intervals.at(geneGroup); // location in bitvector to select in p1
    auto childInt = intervals[geneGroup];
    // iterate over posis: only first posi in child matters. the rest i can ignore ... will be overwritten ..
    int childPos = childInt.first;
    // now: what has changed for all the following gene groups, i.e. gene groups with nbr > 'geneGroup'? 
    // if interval was reduced in child: e.g. from 10 elements to 5 elements -> all start/end pos following shrink by 5
    // if interval has increased in child: e.g. from 7 to 10 elements -> all start/end pos following increase by 3
    int intervalChange = (p1int.second - p1int.first) - (childInt.second - childInt.first); // increase/decrease of child in elements for geneGroup
    // expansion of geneGroup: shift the following geneGroups to the left by abs('intervalChange') bits before setting any bits
    if (intervalChange > 0) { // expansion of geneGroup
        // shift the following gene group bits to the left to make some space 
        shiftLeft(geneGroup, intervalChange);
    }
    for (int i = p1int.first; i <= p1int.second; ++i) {
        chrom.set(childPos, other.chrom.test(i));
        childPos++;
    }

    // std::cout << "geneGroup: " << geneGroup << ", interval difference: " << intervalChange << std::endl;
    // special case for this gene group: only <end> changes
    intervals[geneGroup].second += intervalChange;
    if (intervalChange < 0) {
        // contraction of geneGroup: shift only the following geneGroups to the right by abs('intervalChange') bits after setting the bits 
        shiftRight(geneGroup, intervalChange);
    }
    if (geneGroup != 11) {
        // correct intervals of following gene groups
        for (int i = geneGroup+1; i <= 11; ++i) {
            intervals[i].first += intervalChange;
            intervals[i].second += intervalChange;
        }
    }
    //std::cout << *this << std::endl;
}

/// selects [s,e] from chromosome p 1, takes the rest from chromosome p2
Chromosome createChild(const Chromosome& p1, const Chromosome& p2, int s, int e) {
    Chromosome child = p2;  
    for (int geneGroup = s; geneGroup <= e; ++geneGroup) {
        child.replaceGeneGroup(geneGroup, p1);
    }
    return child;
}

void recombine(const Chromosome& chrom1, const Chromosome& chrom2) {
    // 1. select breakpoint in chromosome in terms of combi idx (1 to 10)
    int bp = rand() % 10 + 1;
    std::cout << "bp at: " << bp << ", parents are: " << std::endl;
    std::cout << chrom1 << std::endl;
    std::cout << chrom2 << std::endl;
    // 2. produce children
    Chromosome c1 = createChild(chrom1, chrom2, bp+1, 11);
    Chromosome c2 = createChild(chrom1, chrom2, 1, bp);
    std::cout << "children are:" << std::endl;
    std::cout << c1 << std::endl;
    std::cout << c2 << std::endl;
}

int scoreChromosome(Chromosome& chrom, const std::vector<int>& diceSequence) {
    // scoring: find best mapping of combination to selected dice and output resulting score
    // greedy: TODO: better -> Find max combi, then next best, then next best .. in global seq
    chromT chromosome = chrom.chrom;
    std::vector<int> roll;
    std::vector<int> combisToConsider(11);
    std::iota(combisToConsider.begin(), combisToConsider.end(), 1);
    int totalScore = 0;
    int rollNbr = 0;
    // score from 'left to right' ... TODO better
    for (int i = 0; i < chromosome.size(); ++i) {
        if (chromosome.test(i)) {
            roll.push_back(diceSequence[i]);
        }
        if (roll.size() == 5) {
            // found a roll
            int maxScore = 0;
            int selCombi = 0;
            for (int combiId : combisToConsider) {
                Combination combi = static_cast<Combination>(combiId);
                int score = scoreRoll(roll, combi);
                if (score >= maxScore) {
                    maxScore = score;
                    selCombi = combiId;
                }
            }
            totalScore += maxScore;
            combisToConsider.erase(std::find(combisToConsider.begin(), combisToConsider.end(), selCombi));
            roll.clear();
            rollNbr += 1;
            std::cout << "rollNbr: " << rollNbr << ", score: " << totalScore << std::endl;
        }
    }
    return totalScore;
}


Chromosome createChromosome(const std::vector<int>& diceSequence) {
    int nbrOfOnes = 11*5; 
    chromT chromosome;
    Chromosome chrom;
    Roll rollSequence(diceSequence);

    // randomly select ones, fulfilling constraints
    // easiest way to fulfill the constraints: just play the game
    // the constraints are very hard to postulate ... to much trouble for initialization i think
    //
    int nbrMoves = 11;
    int nbrRerolls = 2;
    int i = 0; // cur pos in diceSequence
    // setup normal distribution for how many dice should be re-rolled if re-roll occurs
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(3.0,1.5);
    while(nbrMoves--) {
        int initiali = i; // star interval
        //std::cout << "Roll" << std::endl;
        int remainingRerolls = 2;
        rollSequence.roll();
        std::vector<int> usedIdx;
        usedIdx.resize(5);
        std::iota(usedIdx.begin(), usedIdx.end(), i);

        while (remainingRerolls--) {
            if (randProb() >= 0.5) {
                // select indices to re-roll and store
                std::vector<int> idxToReroll;
                double normRerolls = static_cast<int>(round(distribution(generator)));
                int nbrRerolls = normRerolls > 5 ? 5 : (normRerolls < 0 ? 0 : normRerolls);
                std::vector<int> availableIndices(5); // available indices for a reroll
                std::iota(availableIndices.begin(), availableIndices.end(), 0); // 0 to 4
                while (nbrRerolls--) { 
                    int idx = rand() % availableIndices.size();
                    while (std::find(availableIndices.begin(), availableIndices.end(), idx) == availableIndices.end()) {
                        idx = rand() % availableIndices.size(); // resample: ugly but ok
                    }
                    idxToReroll.push_back(availableIndices[idx]);
                    availableIndices[idx] = -1; // make element 'unavailable'
                }
                //std::cout << "Reroll with: " << idxToReroll.size() << std::endl;
                rollSequence.reRoll(idxToReroll);
                for (int r = 0; r < idxToReroll.size(); ++r) {
                    int rerolledIdx = idxToReroll[r];
                    int diceSeqIdx = i + 5 + r;
                    usedIdx[rerolledIdx] = diceSeqIdx;
                }
                i += idxToReroll.size();
            } else {
                break;
            }
        }
        // convert rolled dice into bit vector entry
        for (int idx : usedIdx) {
            //std::cout << "Idx: " << idx << std::endl;
            chromosome.set(idx);
        }
        int newi = *std::max_element(usedIdx.begin(), usedIdx.end()) + 1;
        // store interval of selected elements in the dice sequence
        std::pair<int,int> interval = std::make_pair(initiali, newi - 1);
        chrom.intervals[abs(nbrMoves - 11)] = interval;
        i = newi;
    }
    chrom.chrom = chromosome; 
    return chrom;
}

std::vector<Chromosome> initializePopulation(const std::vector<int>& diceSequence, int populationSize) {
    std::vector<Chromosome> pop(populationSize);
    for (int i = 0; i < populationSize; ++i) {
       pop[i] = createChromosome(diceSequence);
       std::cout << pop[i] << std::endl;
    }
    return pop;
}

void solveGenetic(const std::vector<int>& diceSequence, GParams params) {
    Chromosome chrom = createChromosome(diceSequence);
    std::cout << chrom << std::endl;
    std::vector<Chromosome> chromosomes = initializePopulation(diceSequence, params.populationSize);
    recombine(chromosomes[0], chromosomes[1]);
    // int score = scoreChromosome(chrom, diceSequence);
}
