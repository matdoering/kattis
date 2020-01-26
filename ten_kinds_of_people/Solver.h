#pragma once

#include <string>
#include <vector>

// an answer to the routing problem
enum class Answer {
    BINARY = 0, // if there is a path from source to target only using 0's
    DECIMAL, // if there is a path from source to target only using 1's
    NEITHER // otherwise
};

/* 
 * For an input file specifying a map of 0's and 1's and some queries,
 * identifies whether there is a path from source to target in the map
 * with specific properties (see enum Answer).
 */
class Solver{
    public:
    Solver(std::string sampleFile);
        /// loads a sample file specifying a map and queries to be checked for answers
    std::vector<Answer> solve();
        /// Identifies for each query, whether a solution was possible
    private:
        std::string m_sampleFile;
            /// The sample file to be parsed
};
