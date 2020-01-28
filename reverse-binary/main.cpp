#include <iostream>
#include <bitset>
#include <math.h>       /* log2 */

int main(int argc, char** argv) {
    // parse input
    int nbr;
    std::cin >> nbr;

    // to find revBinary: need to know the left-most bit that is set
    int largestBitPos = (int)(log2(nbr)) + 1; // counting from right to left, starting at 1
    int revBinary = (1 & nbr) ? pow(2, largestBitPos-1) : 0; // nbr interpreted by its reverse binary representation
    for (int i = 1; i < largestBitPos; ++i) { // iterate over individual bits
        if ((int)pow(2, i) & nbr) { // i-th bit is set
            revBinary += pow(2, largestBitPos-i-1);
        }
    }
    std::cout << revBinary << std::endl;
}

