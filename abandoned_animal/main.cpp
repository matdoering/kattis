#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <cassert>
#include <algorithm>

using InfoT = std::unordered_map<std::string, std::set<int>>;

struct SolutionStatus {
    bool any;
    bool unique;
};

// return true if it was possible to find a way to go through the stores, false otherwise
// time limit exceeded at test 7/38
SolutionStatus check(int minShopNbr, int curNbrOfItems, const std::vector<std::string>& expectedItems, const InfoT& shopInfo) {
    std::cout << "min shop nbr: " << minShopNbr << ", items: " << curNbrOfItems << "/" << expectedItems.size() << std::endl;
    // termination conditions:
    if (curNbrOfItems == expectedItems.size()) {
        // a solution path was found (could be ambiguous though
        SolutionStatus status;
        status.any = true;
        status.unique = true;
        //std::cout << "solution found" << std::endl;
        return status;
    }
    const std::string& itemToBuy = expectedItems[curNbrOfItems];
    auto& shopIDs = shopInfo.at(itemToBuy);
    SolutionStatus myStatus;
    myStatus.any = false; // any solution found?
    myStatus.unique = false; // is this a unique solution?
    int uniqueCount = 0;
    for (int id : shopIDs) {
        SolutionStatus status;
        if (id >= minShopNbr) {
            status = check(id, curNbrOfItems+1, expectedItems, shopInfo);
            //std::cout << "found status: " << status.unique << std::endl;
        } else {
            continue;
        }
        if (status.any) {
            myStatus.any = true;
        } 
        if (status.unique && uniqueCount == 0) {
            myStatus.unique = true;
            uniqueCount += 1;
        } else if (status.unique && uniqueCount > 0) {
            //std::cout << " not unique anymore" << std::endl;
            myStatus.unique = false;
        } else if (status.any && !status.unique) {
            myStatus.unique = false;
        }
    }
    return myStatus;
}

std::string solveRecursively(const std::vector<std::string>& boughtItems, const InfoT& shopInfo) {
    // idea: map from boughtItems to shopInfo
    int curShopID = 0;
    std::string answer = "DONT_KNOW";
    SolutionStatus status = check(0, 0, boughtItems, shopInfo);
    if (status.unique) {
        // how to differentiate this case?!
        answer = "unique";
    } else if (status.any) {
        answer = "ambiguous";
    } else {
        answer = "impossible";
    }
    return answer;
}

void parseInput(std::vector<std::string>& boughtItems, InfoT& shopInfo) {
    size_t nbrStores; // nbr of stores to consider for shopping
    std::cin >> nbrStores;
    size_t nShopInfo; // shopping info: {shopID, availableItems}
    std::cin >> nShopInfo;
    while (nShopInfo--) {
        // read shopping info
        int shopID; // order in which shops were visited from 0 to n
        std::string item; // item that is available from shop
        std::cin >> shopID >> item;
        shopInfo[item].emplace(shopID);
    }
    size_t nbrBoughtItems; // nbr of items bought
    std::cin >> nbrBoughtItems;
    boughtItems.resize(nbrBoughtItems);
    for (size_t i = 0; i < boughtItems.size(); ++i) {
        std::cin >> boughtItems[i];
    }
}

void printShopInfo(const InfoT& shopInfo) {
    for (auto it = shopInfo.begin(); it != shopInfo.end(); ++it) {
        std::cout << it->first << ": ";
        auto& shopIds = it->second;
        for (auto itt = shopIds.begin(); itt != shopIds.end(); ++itt) {
            std::cout << *itt << ", ";
        }
        std::cout << std::endl;
    }
}

std::string solveAnalytically(const std::vector<std::string>& boughtItems, InfoT& shopInfo) {
    // idea: eliminate all items that cannot have been possibly bought at a given shop
    int inf = 1 << 30;
    // eliminate from beginning where shopId < curMin
    int curMin = 0; // the maximal minimum value
    for (size_t i = 0; i < boughtItems.size(); ++i) {
        auto& item = boughtItems[i];
        // std::cout << "item: " << item << "(" << i << ")" << std::endl;
        auto& shopIds = shopInfo.at(item);
        for (auto it = shopIds.begin(); it != shopIds.end();) {
            if (*it < curMin) {
                it = shopIds.erase(it);
            } else {
                ++it;
            }
        }
        if (!shopIds.empty()) {
            curMin = std::max(curMin, *shopIds.begin()); // maximum of minimums
        }
    }
    // eliminate from end where shopId > curMax
    int curMax = inf; // the minimal maximal value
    for (int i = boughtItems.size() - 1; i >= 0; i--) {
        auto& item = boughtItems[i];
        //std::cout << "item: " << item << "(" << i << ")" << std::endl;
        auto& shopIds = shopInfo.at(item);
        for (auto it = shopIds.begin(); it != shopIds.end();) {
            if (*it > curMax) {
                it = shopIds.erase(it);
            } else {
                ++it;
            }
        }
        if (!shopIds.empty()) {
            curMax = std::min(curMax, *shopIds.rbegin()); // minimum of maximums
        }
    }
    //printShopInfo(shopInfo);

    // evaluate solution
    bool solutionPossible = true;
    bool uniqueSolution = true;
    for (auto it = shopInfo.begin(); it != shopInfo.end(); ++it) {
        if (it->second.size() == 0) {
            solutionPossible = false;
        }
        if (it->second.size() > 1) {
            uniqueSolution = false;
        }
    }

    if (!solutionPossible) {
        return "impossible";
    } else if (uniqueSolution) {
        return "unique";
    } else {
        return "ambiguous";
    }
}

int main(int argc, char** argv) {
    InfoT shopInfo; // item to list of shop ids offering the item
    std::vector<std::string> boughtItems; // bought items, in order of purchase. all the items are unique.
    parseInput(boughtItems, shopInfo);
    //std::string answer = solveRecursively(boughtItems, shopInfo);
    std::string answer = solveAnalytically(boughtItems, shopInfo);
    std::cout << answer << std::endl;
}
