#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

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
    std::cout << "a: " << a << ", s: " << s << std::endl;
    if (a == 0 || s == 0) {
        return 0.0;
    }
    return std::pow(a, a/s);
}

void printSet(std::vector<int>& subset) {
    for (size_t i =0; i < subset.size(); ++i ){
            std::cout << subset[i] << ", "; 
        }
       std::cout << std::endl;
}

void generateSubsets(std::vector<std::vector<int>>& out, const std::vector<int>& data, int n, int k, int pos, std::vector<int>& subset) {
   // out: all subsets of size k
   // data: construct subsets from these data
   // n: data.size()
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
    for (size_t i = pos; i < n; ++i) {
        subset.push_back(data[i]);
        generateSubsets(out, data, n, k-1, i+1, subset);
        subset.pop_back();
    }
}

double solve(const std::vector<double>& probs) {
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
        std::cout << "subset of size: " << k << std::endl;
        std::vector<std::vector<int>> combis; // all subsets of size k
        std::vector<int> subset;
        generateSubsets(combis, paperIndices, n, k, 0, subset);
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

int main(int argc, char** argv) {
    int nbrPapers;
    std::vector<double> probs;
    readData(nbrPapers, probs);

    std::vector<double> gains(probs.size(), 1);
    double expectedPrdIdx = solve(probs);
    std::cout << expectedPrdIdx << std::endl;
}
