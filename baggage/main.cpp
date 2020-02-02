#include <iostream>
#include <map>
#include <cassert>
#include <math.h>

#define DEBUG 1

enum class ElemType {
    B = 0,
    A = 1,
    EMPTY = 2
};

std::ostream& operator<<(std::ostream& os, ElemType x) {
    switch (x) {
        case ElemType::B:
            os << "B";
            break;
        case ElemType::A:
            os << "A";
            break;
        case ElemType::EMPTY:
            os << " ";
            break;
    }
    return os;
}

struct Data {
    Data(int n ) {
        init(n);
    }
    std::map<int, ElemType> data; // map from used index to element
        /// elements are stored startin from idx 1 to idx K
        // before these elements, there are K empty cells with
        // indices from 0 to K-1
    int N; // number of pairs
        /// there are N pairs of BA's
    int K; // number of non-empty map elements
        // K = N*2
    int startOfRightSide; 
        /// index at which the 'right side' of the array starts
        // the right side is the side where all the B's should accumulate

    void init(int n);
        /// initializes data with alternating entries
    void print();
        /// print out the data
    int move(int from, int to);
        /// move two values 'from' to a free location 'to' (idx in array)
        // outputs the from idx
    std::map<int, ElemType>::iterator findPair(std::pair<ElemType, ElemType> p,
                                               int start, int end, bool isLastMove);
        /// returns iterator to pair p in data
    bool onLeftSide(int idx);
        /// whether idx lies on left side (goal: only A's) or right side of the array
    bool wouldSplit(std::map<int, ElemType>::iterator sIt, ElemType eType, const std::pair<ElemType, ElemType>& foundPair);
        /// whether moval of the elements located at and after sIt would lead to a split
        // sIt: iterator to the position considered for moval
        // eType: the element type to be checked for splitting
        // foundPair: the pair to be moved
        //  e.g. A|AB|A: split of AA on the left
        //  e.g. B|AB|B: split of BB on the right
    bool wouldCreateTriplet(std::map<int, ElemType>::iterator sIt, ElemType eType, const std::pair<ElemType, ElemType>& foundPair, bool isLastMove);
        /// Checks whether a move would create a triplet in the next move
        // triplets cant be moved later on (uneven number of elements) and cause problems in this way
    bool isInvalid(std::map<int, ElemType>::iterator sIt, ElemType eType, const std::pair<ElemType, ElemType>& foundPair, bool isLastMove);
        /// checks for potential splits and triplet creation
        // splits and triplet creation are not allowed
};

bool Data::isInvalid(std::map<int, ElemType>::iterator sIt, ElemType eType, const std::pair<ElemType, ElemType>& foundPair, bool isLastMove) {
    return wouldSplit(sIt, eType, foundPair) || wouldCreateTriplet(sIt, eType, foundPair, isLastMove);
    // wouldCreateTriplet: necessary for n = 8. but destroys n = 6 because triplet is actually never created because next move aint possible
}

bool Data::wouldCreateTriplet(std::map<int, ElemType>::iterator sIt, ElemType eType, const std::pair<ElemType, ElemType>& foundPair, bool isLastMove) {
    if (isLastMove) {
        // we're not doing another AB/BA swap: triplet creation wont occur!
        return false;
    }
    bool triplet = false; // whether triplet would be created in next move
    auto prevIt = std::prev(sIt);
    auto nextIt = std::next(sIt);

    bool prevPossible = std::distance(data.begin(), sIt) >= 2;
    bool nextPossible = std::distance(sIt, data.end()) >= 3;
    if (onLeftSide(sIt->first)) {
        // left side move
        if (foundPair.first == ElemType::A && foundPair.second == ElemType::B) {
            // AB found, check for BB on left 
            if (prevPossible && std::prev(prevIt)->second == eType && prevIt->second == eType) {
                triplet = true;
            }
        } else if (foundPair.first == ElemType::B && foundPair.second == ElemType::A) {
            // BA found, check for BB on right
            if (nextPossible && std::next(nextIt)->second == eType && nextIt->second == eType) {
                triplet = true;
            }
        }
    } else {
        // right side move
        if (foundPair.first == ElemType::A && foundPair.second == ElemType::B) {
            // AB found, check for AA on the right
            if (nextPossible && std::next(std::next(sIt))->second == eType && std::next(sIt)->second == eType) {
                triplet = true;
            }
        } else if (foundPair.first == ElemType::B && foundPair.second == ElemType::A) {
            // BA found, check for AA on the left
            if (prevPossible && std::prev(prevIt)->second == eType && prevIt->second == eType) {
                triplet = true;
            }
        }
    }
    std::cout << "triplet status: " << triplet << std::endl;
    return triplet;
}

bool Data::onLeftSide(int idx) {
    if (idx >= startOfRightSide) {
        return false;
    } else {
        return true;
    }
}

bool Data::wouldSplit(const std::map<int, ElemType>::iterator sIt, ElemType eType, const std::pair<ElemType, ElemType>& foundPair) {
    bool split = false;

    auto prevIt = std::prev(sIt);
    auto nextIt = std::next(sIt);

    if (sIt != data.begin() && (foundPair.first == eType && prevIt->second == eType)) {
        // BB on the left of found pair would be split
        split = true;
    } else if (nextIt != data.end() && std::next(nextIt) != data.end() && foundPair.second == eType && std::next(nextIt)->second == eType) {
        // BB on the right of found pair would be split
        split = true;
    }
    std::cout << "split status: " << split << std::endl;
    return split;
}

std::map<int, ElemType>::iterator Data::findPair(std::pair<ElemType, ElemType> p,
        int start, int end, bool isLastMove) {
    #if DEBUG
        std::cout << "Searching: " << p.first << p.second << " in [" << start << "," << end << "]" << std::endl;
    #endif
    bool preventSplits = p.first != p.second;
    auto sIt = data.find(start);
    assert(sIt != data.end()); // element must exist
    assert(data.find(end) != data.end()); // element must exist
    auto eIt = std::next(data.find(end)); // closed interval to the right
    //auto eIt = data.find(end); // open interval to the right
    for (; sIt != eIt; ++sIt) {
        if (sIt == data.end()) {
            break;
        }
        auto foundPair = std::make_pair(sIt->second, std::next(sIt)->second);
        std::cout << "found pair: " << foundPair.first << ", " << foundPair.second << std::endl;
        if (foundPair == p && !preventSplits) {
            return sIt;
        } else if (foundPair == p && preventSplits) {
            // check whether foundPair move is not allowed
            ElemType eType;
            if (onLeftSide(sIt->first) && isInvalid(sIt, ElemType::B, foundPair, isLastMove)) {
                // do not cause trouble on the left side
                continue;
            } else if (isInvalid(sIt, ElemType::A, foundPair, isLastMove)) {
                // do not cause trouble on the right side
                continue;
            }
            return sIt;
        }
    }
    return data.end();
}

int Data::move(int from, int to) {
    auto from1 = data.find(from);
    auto from2 = data.find(from+1);
    assert(from1 != data.end());
    assert(from2 != data.end());

    auto to1 = data.find(to);
    assert(to1 != data.end());
    auto to2 = data.find(to+1);
    assert(to2 != data.end());

    assert(to1->second == ElemType::EMPTY &&
           to2->second == ElemType::EMPTY);
    assert(from1->second != ElemType::EMPTY &&
           from2->second != ElemType::EMPTY);
   
    to1->second = from1->second;
    to2->second = from2->second;
    from1->second = ElemType::EMPTY;
    from2->second = ElemType::EMPTY;

    std::cout << from << " to " << to << std::endl;
    #if DEBUG
        print();
    #endif
    return from; // returns the last input source
}

void Data::print() {
    std::map<int, int> charSizes; // for padding
    int printFromIdx = -2;
    for (auto it = data.begin(); it != data.end(); ++it) {
        std::string sep = "|";
        if (it->first == N - 2) {
            sep = "#";
        }
        charSizes[it->first] = std::to_string(it->first).size();
        if (it->first >= printFromIdx) {
            std::cout << it->first << sep;
        }
    }
    std::cout << std::endl;

    for (auto it = data.begin(); it != data.end(); ++it) {
        int charSize = charSizes.at(it->first);
        std::string padding = std::string(charSize - 1, ' ');
        std::string sep = "|";
        if (it->first == N - 2) {
            sep = "#"; 
        }
        if (it->first >= printFromIdx) {
            std::cout << it->second << padding << sep;
        }
    }
    std::cout << std::endl;
}

void Data::init(int n) {
    int N = n*2*2;
    for (int i = 0; i < N; ++i) {
        ElemType elemType;
        if (i < 2*n) {
            // init empty cells
            elemType = ElemType::EMPTY;
        } else {
            // alternating elements
            elemType = static_cast<ElemType>(i % 2);
        }
        int usedIdx = i - 2*n + 1;
        data[usedIdx] = elemType;
    }
    K = 2*n;
    this->N = n;
    startOfRightSide = n - 1;
}

int solveIter(Data& data, int stopIter,
              std::pair<ElemType, ElemType> item1,
              std::pair<ElemType, ElemType> item2,
              int lastSource) {
    int moveIters = 0;
    for (; moveIters < stopIter; ++moveIters) {
        // solve second phase (same for even)
        // determine side with empty cells
        int s,e;
        std::pair<ElemType, ElemType> item;
        if (data.onLeftSide(lastSource)) {
            // empty cells on the left side -> search on the right side
            s = data.startOfRightSide;
            e = data.K;
            item = item1;
        } else {
            // empty cells on the right side -> search on the left side
            s = -1;
            e = data.startOfRightSide -1;
            item = item2;
        }
        bool isLastMoveInPhase = moveIters+1 == stopIter;
        auto pairIt = data.findPair(item, s, e, isLastMoveInPhase);
        if (pairIt == data.data.end()) {
            #if DEBUG
            std::cout << "Error: Did not find pair!" << std::endl;    
            #endif
            break;
        } else {
            #if DEBUG
            std::cout << "Found pair at: " << pairIt->first << std::endl;
            #endif
            lastSource = data.move(pairIt->first, lastSource);
        }
    }
    return lastSource;
}

void solveEven(Data& data, int lastSource) {
    #if DEBUG
    std::cout << "SECOND STEP" << std::endl;
    #endif
    lastSource = data.move(data.K-5, lastSource); // move BA to right
    #if DEBUG
    std::cout << std::endl;
    std::cout << "FIRST PHASE" << std::endl;
    #endif

    // search AB/BA that doesnt split pairs
    lastSource = solveIter(data, ceil(static_cast<double>(data.N) / 2) - 2, 
              std::make_pair(ElemType::B, ElemType::A),
              std::make_pair(ElemType::A, ElemType::B), lastSource); // used lastSource before
    #if DEBUG
    std::cout << std::endl;
    std::cout << "SECOND PHASE" << std::endl;
    #endif
    // search AA/BB 
    lastSource = solveIter(data, data.N / 2, std::make_pair(ElemType::A, ElemType::A),
              std::make_pair(ElemType::B, ElemType::B), lastSource);

 
}
void solveUneven(Data& data, int lastSource) {
    #if DEBUG
    std::cout << "SECOND STEP" << std::endl;
    #endif
    lastSource = data.move(data.N-2, lastSource);
    #if DEBUG
    std::cout << std::endl;
    std::cout << "FIRST PHASE" << std::endl;
    #endif
    if (data.N == 3) {
        // special case
        data.move(-1, lastSource);
        data.move(5, -1);
    } else {
        // search AB/BA that doesnt split pairs
        lastSource = solveIter(data, ceil(static_cast<double>(data.N) / 2) - 2, 
                  std::make_pair(ElemType::A, ElemType::B),
                  std::make_pair(ElemType::B, ElemType::A), lastSource);
        #if DEBUG
        std::cout << std::endl;
        std::cout << "SECOND PHASE" << std::endl;
        #endif
        // search AA/BB 
        lastSource = solveIter(data, data.N / 2, std::make_pair(ElemType::A, ElemType::A),
                  std::make_pair(ElemType::B, ElemType::B), lastSource);
    }
}

void solve(Data& data) {
    // first move
    #if DEBUG
    std::cout << "FIRST STEP" << std::endl;
    #endif
    int lastSource = data.move(data.K-2, -1);

    if (data.N % 2 == 0) {
        // even
        solveEven(data, lastSource);
    } else {
        // odd
        solveUneven(data, lastSource);
    }
}

int main(int argc, char** argv) {
    // parse input
    int n;
    std::cin >> n;
    // init data
    Data data(n);
    #if DEBUG
    data.print();
    #endif
    solve(data);
    
}
