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
    os << "\nScore: " << c.fitness;
    return os;
}

//int selectedIdx = rand() % (interval.second - interval.first + 1) + interval.first; // idx within range, TODO: use uniform int distribution
// again: two cases: 1. remove a bit -> need to expand intervals of gene group to the left. 2. add a bit -> need to reduce intervals of gene group.
// TODO: mutations need to be within the rules of the game:
// per gene group we can have at most 10 0's -> check when changing a 1 to a 0
// per chromosome, we need _exactly_ 55 1's. so, we cant just add/remove a single 1 to be in a valid state...
// idea: add a 1, remove a 1, if possible. problem: 11111 -> no change possible here
// for 101111: would need to change to 1|1|1111, then remove a random 1, e.g.: 1|1|1101
// other approach: if we remove a 1 -> gene group needs to expand
// if we remove a 1 -> gene group needs to contract
// TODO: still need to remove/add a 1 to fix the chromosome's number of 1's
void Chromosome::mutateWithin(double mutProb, const std::vector<int>& diceSequence) {
    //TODO: implement me
    bool wasMutated = false;
    for (int i = 1; i <= 11; ++i) { // choose gene group (combination)
        if (randProb() > mutProb) {
            continue;
        }
        const auto& interval = intervals[i];
        // check whether mutation within is possible -> at least one 0 needs to exist to allow for balance (same no of 1's remain)
        std::vector<int> zeroIdx; 
        std::vector<int> oneIdx;
        for (int j = interval.first; j <= interval.second; ++j) {
            if (chrom.test(j)) {
                oneIdx.push_back(j);
            } else {
                zeroIdx.push_back(j);
            }
        }
        if (zeroIdx.size() == 0) {
            // all 1's -> cannot mutate anything within gene group
            continue;
        }
        // select idx to mutate
        int mutIdxZero = rand() % zeroIdx.size(); // change from 0 -> 1
        int mutIdxOne = rand() % oneIdx.size(); // change from 1 -> 0
        assert(!chrom.test(zeroIdx[mutIdxZero]));
        assert(chrom.test(oneIdx[mutIdxOne]));
        chrom.set(zeroIdx[mutIdxZero], true);
        chrom.set(oneIdx[mutIdxOne], false);
        wasMutated = true;
        // std::cout << "Mutated!" << std::endl;
    }
    if (wasMutated) {
        // remember to update the fitness score of the chromosome if it has been mutated
        score(diceSequence);
    }
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
Chromosome createChild(const Chromosome& p1, const Chromosome& p2, int s, int e, const std::vector<int>& diceSequence) {
    Chromosome child = p2;  
    for (int geneGroup = s; geneGroup <= e; ++geneGroup) {
        child.replaceGeneGroup(geneGroup, p1);
    }
    child.score(diceSequence);
    return child;
}

std::vector<Chromosome> recombine(const Chromosome& chrom1, const Chromosome& chrom2, const std::vector<int>& diceSequence) {
    // 1. select breakpoint in chromosome in terms of combi idx (1 to 10)
    int bp = rand() % 10 + 1;
    // 2. produce children
    Chromosome c1 = createChild(chrom1, chrom2, bp+1, 11, diceSequence);
    Chromosome c2 = createChild(chrom1, chrom2, 1, bp, diceSequence);
    /*
    std::cout << "bp at: " << bp << ", parents are: " << std::endl;
    std::cout << chrom1 << std::endl;
    std::cout << chrom2 << std::endl;

    std::cout << "children are:" << std::endl;
    std::cout << c1 << std::endl;
    std::cout << c2 << std::endl;
    */
    return {c1, c2};
}

int scoreChromosomeOld(Chromosome& chrom, const std::vector<int>& diceSequence) {
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
            auto usedCombiIt = std::find(combisToConsider.begin(), combisToConsider.end(), selCombi);
            if (usedCombiIt != combisToConsider.end()) { // TODO: investigate when this occurs / rewrite algo
                combisToConsider.erase(usedCombiIt);
            }
            roll.clear();
            rollNbr += 1;
            //std::cout << "rollNbr: " << rollNbr << ", score: " << totalScore << std::endl;
        }
    }
    return totalScore;
}

// greedy scoring for each combination
void Chromosome::score(const std::vector<int>& diceSequence) {
    std::vector<int> remainingGeneGroups(11);
    std::iota(remainingGeneGroups.begin(), remainingGeneGroups.end(), 1);
    int totalScore = 0;
    // order combinations from highest constraints/gain to lowest
    // order has an extremely high impact (e.g. shift of max score from 120 to 180 in genetic algo)
    std::vector<Combination> combinations = {Combination::FIVE_OF_A_KIND,
        Combination::FOUR_OF_A_KIND, Combination::FULL_HOUSE,
        Combination::SEQUENCE, Combination::SIXES, Combination::FIVES,
        Combination::FOURS, Combination::THREES, Combination::TWOS,
        Combination::ONES, Combination::CHANCE};
    for (int combiId = 0; combiId < combinations.size(); ++combiId) {
        Combination combi = combinations[combiId];
        int maxScore = 0;
        int selGeneGroup = 0;
        for (int i = 0; i < remainingGeneGroups.size(); ++i) {
            int geneGroup = remainingGeneGroups[i];
            const auto& interval = intervals[geneGroup];
            std::vector<int> sel(diceSequence.begin() + interval.first, diceSequence.begin() + interval.second + 1);
            std::vector<int> roll;
            for (int j = interval.first; j <= interval.second; ++j) {
                if (chrom.test(j)) {
                    roll.push_back(diceSequence[j]);
                }
            }
            //printRoll(roll);
            int score = scoreRoll(roll, combi);
            if (score >= maxScore) {
                maxScore = score;    
                selGeneGroup = geneGroup;
            }
        }
        if (maxScore != 0) {
            // if combi cannot be found (score: 0) -> keep selected set in remaining groups to use these dice in a more meaningful way
            remainingGeneGroups.erase(std::find(remainingGeneGroups.begin(), remainingGeneGroups.end(), selGeneGroup));
        }
        //std::cout << "Max score for combination " << combi << " was: " << maxScore << std::endl;
        totalScore += maxScore;
    }
    fitness = totalScore;
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

    // determine score of chromosome
    chrom.score(diceSequence);
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

void printPopulation(const std::vector<Chromosome>& population) {
    for (const auto& c : population) {
        std::cout << c << std::endl;
    }
}

bool compareChromosome(const Chromosome& c1, const Chromosome& c2) {
    return c1.fitness > c2.fitness;
}

void removeLeastFit(std::vector<Chromosome>& population, int nRemove) {
    // ... could also have the population be always sorted (prio queue) .. TODO?
    sort(population.begin(), population.end(), compareChromosome);
    population.erase(population.end()-nRemove, population.end());
}

void mutateWithin(std::vector<Chromosome>& population, double mutProb, const std::vector<int>& diceSequence) {
    for (Chromosome& c : population) {
        c.mutateWithin(mutProb, diceSequence);
    }
}

void solveGenetic(const std::vector<int>& diceSequence, GParams params) {
    std::vector<Chromosome> population = initializePopulation(diceSequence, params.populationSize);
    std::cout << "Population initialized!" << std::endl << std::flush;
    int iteration = 0;
    /////// START
    while (true) { // TODO: stop based on change in fitness between iterations and nbr of iterations
        // 1. Recombination
        // select random pair of parents
        //std::cout << "Population size is: " << population.size() << std::endl;
        //printPopulation(population);
        int idx1 = rand() % population.size();
        int idx2 = rand() % population.size(); // TODO: re-choose if the same
        std::vector<Chromosome> children = recombine(population[idx1], population[idx2], diceSequence);
        // 2. Mutation on children
        mutateWithin(children, params.mutationProb, diceSequence);
        // add children to population
        for (const auto& c : children) {
            population.push_back(c);
        }

        // 3. Selection
        // Idea: just remove the least fit individual(s). for constant population size: always remove the 2 least fit members
        // TODO use this as param in GParams
        int nRemove = 2;
        removeLeastFit(population, nRemove); // TODO: improve scoring
        iteration += 1;
        std::cout << "Iteration " << iteration << ", max fitness: " << population[0].fitness << std::endl;
    }
}
