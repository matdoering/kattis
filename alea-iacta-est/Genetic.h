#pragma once

#include <vector>
#include <bitset>
#include <unordered_map>
#include <iostream>

using chromT = std::bitset<15*11>;


/* A chromosome represents a sequence of decisions in the 11*15 long
 * sequence of possible dice rolls of the random number generator.
 * Use of a dice is indicated by '1'. A dice that is not used (e.g. due to re-rolls)
 * is indicated by '0'.
 *
 * Since there are 11 combinations to be selected (for each of which 3 rolls can be performed at most)
 * each of the areas in the chromosome are delineated according to the combination borders.
 * e.g. |01100111| has the interval [0, 7]
 */ 
struct Chromosome {
    chromT chrom; /// the vector of dice rolls (1/0)
    std::unordered_map<int, std::pair<int,int>> intervals; // map from combination (1 to 11) to closed interval in the dice sequence ('chrom')
    void replaceGeneGroup(int geneGroup, const Chromosome& other); // replaces geneGroup with gene group from other
    void shiftLeft(int geneGroup, int by); // shift entries to the left which are greater than gene group
    void shiftRight(int geneGroup, int by); // shift entries to the right which are greater than gene group

};

std::ostream& operator<<(std::ostream& os, const Chromosome& c);

// for recombination: only recombine "sets of five genes" that make up a dice roll so as to
// ensure that jahtzee constraints hold
// only recombine in the area where both values have something other than 0s (left bit vector is mostly 0)
// TODO: for recombination: may / should change data structure of chromosome
// -> need to have a function to select the blocks (5 grouping) ...
// blocks are dynamic so this is just a getter: getblock(int blockNr) -> interval (i,j)

void recombine(const Chromosome& chrom1, const Chromosome& chrom2);
    // select recombination idx (consider position of leftmost 1 in both strings)
    // modify recombination idx: ensure that 5 genes are transferred
    // output offspring chromosomes
    // example (read from l to r)
    // 0000111011|0110111
    // 11111|010111001
    // child 1:
    // 0000111011|010111001|
    // child 2:
    // 11111|0110111|
    // basically: recombination specifies which of the 11 dice rolls to exchange
    // TODO: multiple dice rolls could also be switched (how many?)
    // recombination: let 1 to 11 indicate each combination
    // input: breakpoint at 05
    // 11|10|09|08|07|06|05|04|03|02|01|
    // offspring 1:
    // 11|10|09|08|07|06| < 06: from other parent
    // offspring 2:
    // >06 from other|06|05|04|03|02|01

int scoreChromosome(Chromosome& chromosome, const std::vector<int>& diceSequence);
Chromosome createChromosome(const std::vector<int>& diceSequence);

/* Defines an initial population of chromosomes*/
std::vector<Chromosome> initializePopulation(const std::vector<int>& diceSequence, int popSize);

/* State of genetic algorithm */
struct GState{
    std::vector<Chromosome> population;
    int generationNbr;
};

/* Params for genetic algorithm*/
struct GParams {
    int populationSize = 50;
};


void solveGenetic(const std::vector<int>& diceSequence, GParams params);
