#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iomanip>      // std::setprecision

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

void generateSubsets(std::vector<std::vector<int>>& out, const std::vector<int>& data, int k, int pos, std::vector<int>& subset) {
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
        subset.push_back(data[i]);
        generateSubsets(out, data, k-1, i+1, subset);
        subset.pop_back();
    }
}


// for n = 2: 
// if we have k = 2 -> just return p1 * p2
// for k = 1 -> p1 * (1-p2) + p2 * (1-p1)
// for k = 0 -> 0
//
// for n = 3 and k = 2
// 1st&2nd elem chosen= p1 * p2 * (1-p3) + 
// 1st&3rd elem chosen= p1 * p3 * (1-p2) 
// 2nd&3rd elem chosen= p2 * p3 * (1-p1)
double subsetProbability(const std::vector<double>& probs, int k, int n) {
    // k: nbr selected
    // n: nbr available
    std::vector<std::vector<int>> combis; // all subsets of size k
    std::vector<int> subset;
    std::vector<int> indices(n);
    for (int i =0; i < indices.size(); ++i) {
        indices[i] = i;
    }
    generateSubsets(combis, indices, k, 0, subset); // TODO: performance -> pre-generate all subsets earlier
    //std::cout << "nbr of subsets: " << combis.size() << std::endl;
    double P = 0.0; // probability of subset
    for (auto& subset: combis) {
        // prob of chosen and non chosen elements
        double p = 1.0;
        for (int i : indices) {
            if (find(subset.begin(), subset.end(), i) != subset.end()) {
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
        //std::cout << "##submission size: " << nSubmitted << std::endl;
        double prdIdx = 0.0;
        for (size_t nAccepted = 0; nAccepted <= nSubmitted; ++nAccepted) {
            // select publications to be considered as accepted
            double p = subsetProbability(probs, nAccepted, nSubmitted);
            //std::cout << "P: " << p << std::endl;
            double ePrdIdx = p*productivityIndex(nAccepted, nSubmitted);
            prdIdx += ePrdIdx;
            //std::cout << "nbr accepted: " << nAccepted << "/" << nSubmitted << ", prdIdx: " << ePrdIdx << std::endl;
        }
        //std::cout << "==> total prd idx: " << prdIdx << std::endl;
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
    double expectedPrdIdx = solve(probs);
    std::cout << std::setprecision(20) << expectedPrdIdx << std::endl;
    /*
    std::cout << "TEST" << std::endl;
    std::cout << subsetProbability(probs, 1, 2) << std::endl;
    */
}
