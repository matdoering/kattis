#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iomanip>      // std::setprecision
#include <bitset>

static const int MAX_PAPERS = 100; // the maximum allowed nbr of papers accepted as an input

void readData(int& nbrPapers, std::vector<double>& probs) {
    std::cin >> nbrPapers;
    while (nbrPapers--) {
        int probPercentage;
        std::cin >> probPercentage; // acceptance probability for paper
        probs.push_back((static_cast<double>(probPercentage)) / 100);
    }
}

double expectedValue(const std::vector<double>& probs, const std::vector<double>& values,
                    const std::vector<int>& indices) {
    // only vector entries with idx in indices are considered

    // TODO: this is not really an expected value if probabilities don't sum up to one
    double eVal = 0.0;
    for (int i : indices) {
        eVal += probs[i]*values[i];
    }
    return eVal;
}

void printSet(std::vector<int>& subset) {
    for (size_t i =0; i < subset.size(); ++i ){
            std::cout << subset[i] << ", "; 
        }
       std::cout << std::endl;
}

void generateSubsets(std::vector<std::bitset<MAX_PAPERS>>& out, const std::vector<int>& data, int k, int pos, std::bitset<MAX_PAPERS>& subset) {
   // out: all subsets of size k
   // data: construct subsets from these data
   // k: remaining elements that need to be added to subset
   // pos: position in data to start iteration
   // subset: current constructed subset

   if (k == 0) {
       // we're done
       out.push_back(subset);
       //printSet(subset);
       return;
    }
    // recursion
    for (size_t i = pos; i < data.size(); ++i) {
        subset.set(data[i]);
        generateSubsets(out, data, k-1, i+1, subset);
        subset.reset(data[i]);
    }
}



// calcaulates the probability to have k accepted papers
// when n papers are submitted assuming that each papers
// has an acceptance probability as provided in probs
// // this calculates the Poisson binomial probability
// https://en.wikipedia.org/wiki/Poisson_binomial_distribution
// approximation with fourier transform would be possible
double subsetProbability(const std::vector<double>& probs, int k, int n) {
    // k: nbr selected
    // n: nbr available
    //
    std::vector<std::bitset<MAX_PAPERS>> combis; // all subsets of size k
    std::bitset<MAX_PAPERS> subset;
    std::vector<int> indices(n);
    for (int i =0; i < indices.size(); ++i) {
        indices[i] = i;
    }
    //std::cout << "generating subsets ... " << std::endl;
    generateSubsets(combis, indices, k, 0, subset);
    //std::cout << "subsets generated!" << std::endl;
    //std::cout << "nbr of subsets: " << combis.size() << std::endl;
    double P = 0.0; // probability of subset
    for (auto& subset: combis) {
        // prob of chosen and non chosen elements
        double p = 1.0;
        for (int i : indices) {
            if (subset[i]) {
                // paper included
                p *= probs[i];
            } else {
                // paper not included
                p *= (1-probs[i]);
            }
        }
        //std::cout << "p: " << p << std::endl;
        P += p;
    }
    return P;
}
/*
double choose(int n, int k) {
    // from n choose k
    assert(k <= n);
    if (k == 0) {
        return 1;
    }
    return ((double)n/k) * choose(n-1, k-1);
}
*/

double productivityIndex(double a, int s) {
    // a: nbr accepted papers
    // s: nbr submitted papers
    assert(a <= s);
    assert(a >= 0 && s >=0);
    if (a == 0 || s == 0) {
        return 0.0;
    }
    return std::pow(a, a/s);
}

/*
double solveWithExpVal(const std::vector<double>& probs) {
    // incorrect
   std::vector<double> gains(probs.size(), 1);
   std::vector<int> paperIndices(probs.size());;
   for (size_t i = 0; i < paperIndices.size(); ++i) {
       paperIndices[i] = i;
    }
   
   // generate all n choose k combinations of papers to be submitted
    int n = probs.size();
    double maxPrdIdx = 0.0;    
    for (size_t k = 1; k <= probs.size(); ++k) {
        // generate all paper selections of size k
        std::cout << "##subset of size: " << k << std::endl;
        std::vector<std::vector<int>> combis; // all subsets of size k
        std::vector<int> subset;
        generateSubsets(combis, paperIndices, k, 0, subset);
        for (auto& subset : combis) {
            // calculate expected nbr of accepted papers
           for (size_t i =0; i < subset.size(); ++i ){
                std::cout << subset[i] << ", "; 
            }
            std::cout << std::endl;
            double eNbrAccepted = expectedValue(probs, gains, subset); // expected number of accepted papers
            // calculate the productivity index based on expected nbr
            double prdIdx = productivityIndex(eNbrAccepted, k);
            std::cout << "nbr accepted: " << eNbrAccepted << ", prdIdx: " << prdIdx << std::endl;
            if (prdIdx > maxPrdIdx) {
                maxPrdIdx = prdIdx;
            }
        }
    }
    return maxPrdIdx;
}
*/

double solve(const std::vector<double>& probs) {
    // incorrect
   std::vector<double> gains(probs.size(), 1);
   std::vector<int> paperIndices(probs.size());;
   for (size_t i = 0; i < paperIndices.size(); ++i) {
       paperIndices[i] = i;
    }
   
   // generate all n choose k combinations of papers to be submitted
    int n = probs.size();
    double maxPrdIdx = 0.0;    


    for (size_t nSubmitted = 1; nSubmitted <= probs.size(); ++nSubmitted) {
        // generate all paper selections of size k
        std::cout << "n=" << nSubmitted << std::endl;
        double prdIdx = 0.0;
        for (size_t nAccepted = 1; nAccepted <= nSubmitted; ++nAccepted) {
            std::cout << "k=" << nAccepted << std::endl;
            // select publications to be considered as accepted
            double p = subsetProbability(probs, nAccepted, nSubmitted);
            //std::cout << "P: " << p << std::endl;
            double ePrdIdx = p*productivityIndex(nAccepted, nSubmitted);
            prdIdx += ePrdIdx;
            std::cout << "nbr accepted: " << nAccepted << "/" << nSubmitted << ", prdIdx: " << ePrdIdx << std::endl;
        }
        std::cout << "==> total prd idx: " << prdIdx << std::endl;
        if (prdIdx > maxPrdIdx) {
            maxPrdIdx = prdIdx;
        }
    }
    return maxPrdIdx;
}

// we want an iterative algorithm to come up with the probabilities
// p(k,n) for having k papers from n accepted
// -> in every iteration we increase n and we cycle through all k's
//
double solveFast(const std::vector<double>& probs) {
    // basis case: only 1 probability value provided as input
    double maxPrdIdx = probs[0] * productivityIndex(1, 1);
    std::vector<double> P = {probs[0], 1 - probs[0]}; // initial probabilities
    // all other cases:
    for (int n = 1; n < probs.size(); ++n) {
        //std::cout << "n: " << n << std::endl;
        double curProb = probs[n];
        std::vector<double> curProbs = {P[0] * curProb}; // prob of choosing the 1st elem
        for (int k = 0; k < n; ++k) {
            // prob of choosing the  ... TODO
            curProbs.push_back(P[k]*(1-curProb) + P[k+1] * curProb);
        }
        curProbs.push_back(P[n]*(1-curProb));
        /*
        std::cout << "cur probs:" << std::endl;
        for (int i = 0; i < curProbs.size(); ++i) {
            std::cout << curProbs[i] << ", ";
        }
        std::cout << std::endl;
        */
        P = curProbs; // update P
        double prdIdx = 0.0;
        for (int j =0; j < P.size(); ++j) { // j: nbr accepted papers
            prdIdx += (P[j] * productivityIndex(n+1-j, n+1)); // submitted nbr of papers i n+1
        }
        // std::cout << "prd idx is: " << prdIdx << std::endl;
        if (prdIdx > maxPrdIdx) {
            maxPrdIdx = prdIdx;
        }
    }
    return maxPrdIdx;
}

int main(int argc, char** argv) {
    int nbrPapers;
    std::vector<double> probs;
    readData(nbrPapers, probs);
    // sort the probabilities from highest to lowest
    // -> needed later for paper selection
    std::sort(probs.begin(), probs.end(), std::greater<double>());

    std::vector<double> gains(probs.size(), 1);
    //double expectedPrdIdx = solve(probs);
    double expectedPrdIdx = solveFast(probs);
    std::cout << std::setprecision(20) << expectedPrdIdx << std::endl;
    /*
    std::cout << "TEST" << std::endl;
    std::cout << subsetProbability(probs, 1, 2) << std::endl;
    */
}
