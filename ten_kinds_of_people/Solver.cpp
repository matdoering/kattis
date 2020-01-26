#include "Solver.h"

#include <array>
#include <fstream>
#include <iostream> // debug only
#include <sstream>
#include <utility>
#include <cassert>
#include <map>
#include <queue>
#include <chrono> // debug only
#include <unordered_set>
#include <bits/stdc++.h> 

#define DEBUG 0


namespace {

// A hash function used to hash a pair of any kind 
struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const std::pair<T1, T2>& p) const
    { 
        auto hash1 = std::hash<T1>{}(p.first); 
        auto hash2 = std::hash<T2>{}(p.second); 
        return hash1 ^ hash2; 
    } 
};

/* GRAPH */


/// Whether a node in the graph was visited yet
enum class Status {
    NOT_VISITED,
    VISITED
};

/// A node in the graph
struct Node {
    std::pair<int,int> id;
    std::vector<Node*> next;
    Status status;
    int mapValue; // 0 or 1
    void addNeighbor(Node* n, int nValue);
        /// add neighbor node n to the current node if nValue = n.mapValue

};

std::ostream& operator<<(std::ostream& os, Node n) {
    return os << n.id.first << "," << n.id.second << " (" << n.mapValue << ")";
}

void Node::addNeighbor(Node* n, int nValue) {
    assert(nValue == 1 || nValue == 0);
    if (mapValue == nValue) {
        // only in this case is transition allowed
        next.push_back(n);
    }
}


/*
 * Represents a query from a point 'from' to another point 'to'
 */
struct Query {
    std::pair<int,int> from;
    std::pair<int,int> to;
};

void operator>>(std::istringstream& iss, Query& q) {
    size_t x1, y1, x2, y2;
    iss >> x1 >> y1 >> x2 >> y2;
    q.from = std::make_pair(x1, y1);
    q.to = std::make_pair(x2, y2); 
}

/* Graph structure for searching a path */
class Graph {
    public:
        void buildFromMap(const std::vector<std::vector<int>>& map, size_t nrow, size_t ncol);
            /// Construct a graph from a binary map where N/E/S/W movement is possible
        Answer search(const Query& q);
            /// breadth-first search from source to target
    private:
        std::map<std::pair<int,int>, Node> m_nodes;
            /// maps coordinates in the parsed map to graph nodes
        void resetState();
            /// Resets the state of the nodes in the graph such that all nodes are unvisited
};


void Graph::resetState() {
    for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it) {
        it->second.status = Status::NOT_VISITED;
    }
}

/// breadth-first search: assumes that graph structure has been precomputed
bool bfs(Node& sourceNode, Node& targetNode) {
    std::queue<Node*> nextNodes;
    assert(sourceNode.status == Status::NOT_VISITED);
    sourceNode.status = Status::VISITED;
    nextNodes.emplace(&sourceNode);
    while (!nextNodes.empty()) {
        auto n = nextNodes.front();
        nextNodes.pop(); // remove the front element
        if (n->id == targetNode.id) {
            return true; // target reached on a valid path
        }
        for (auto nn : n->next) {
            if (nn->status == Status::NOT_VISITED) {
                nextNodes.emplace(nn);
                nn->status = Status::VISITED;
            }
        }
    }
    return false;
}

/// in-place breadth-first search: doesnt require precomputed graph structure
bool bfsInPlace(const Query& q, const std::vector<std::vector<int>>& map) {
    std::queue<const std::pair<int,int>*> nextNodes; // nodes to be considered: pointers refer to elements of 'closedList'
    std::unordered_set<std::pair<int,int>, hash_pair> closedList; // already visited nodes
    auto sourceIt = closedList.emplace(q.from).first; 

    nextNodes.emplace(&*sourceIt); // store pointer to pair
    int nrow = map.size();
    int ncol = map[0].size(); // assume map is non-empty
    while (!nextNodes.empty()) {
        //std::cout << "remaining nodes: " << nextNodes.size() << std::endl;
        auto n = nextNodes.front();
        nextNodes.pop(); // remove the front element
        //std::cout << "at: " << n << std::endl << std::flush;
        if (*n == q.to) { // compare coordinates here ... can be improved?! TODO
            return true; // target reached on a valid path
        }
        // store neighbors
        int i = n->first - 1;
        int j = n->second - 1;
        if (j > 0 && map[i][j-1] == map[i][j]) {
            // left neighbor
            auto emplRes = closedList.emplace(i+1, j); // try to emplace this node
            if (emplRes.second) { // emplace occured -> we haven't yet visited this node
                nextNodes.emplace(&*emplRes.first);
            }
        }
        if (i > 0 && map[i-1][j] == map[i][j]) {
            // top neighbor
            auto emplRes = closedList.emplace(i, j+1);
            if (emplRes.second) {
                // not yet visited
                nextNodes.emplace(&*emplRes.first);
            }
        }
        if (j < ncol-1 && map[i][j+1] == map[i][j]) {
            // right neighbor
            auto emplRes = closedList.emplace(i+1, j+2);
            if (emplRes.second) {
                // not yet visited
                nextNodes.emplace(&*emplRes.first);
            }
        }
        if (i < nrow-1 && map[i+1][j] == map[i][j]) {
            // bottom neighbor
            auto emplRes = closedList.emplace(i+2, j+1);
            if (emplRes.second) {
                // not yet visited
                nextNodes.emplace(&*emplRes.first);
            }
        }
    }
    return false;
}

/// in-place breadth-first search using knowledge from all queries
bool bfsInPlaceMem(const Query& q, const std::vector<std::vector<int>>& map,
                   std::vector<std::vector<int>>& reachableMap, int reachMarker) {
    std::queue<std::pair<int,int>> nextNodes; // nodes to be considered
    nextNodes.emplace(q.from); // store pointer to pair

    int nrow = map.size();
    int ncol = map[0].size(); // assume map is non-empty
    while (!nextNodes.empty()) {
        auto n = nextNodes.front();
        nextNodes.pop(); // remove the front element
        // store neighbors
        int i = n.first - 1;
        int j = n.second - 1;
        if (reachableMap[i][j]) {
            // we already have a reachability marker other than 0 for this node.
            // -> don't overwrite it!
            // -> don't need to visit node again because it won't help us reach the goal (other equivalence class)
            continue;
        }
        reachableMap[i][j] = reachMarker; // TODO: use reachableMap as closed list -> if we have visited node previously, we don't need to visit again
        if (j > 0 && map[i][j-1] == map[i][j]) {
            // left neighbor
            nextNodes.emplace(i+1, j);
        }
        if (i > 0 && map[i-1][j] == map[i][j]) {
            // top neighbor
            nextNodes.emplace(i, j+1);
        }
        if (j < ncol-1 && map[i][j+1] == map[i][j]) {
            // right neighbor
            nextNodes.emplace(i+1, j+2);
        }
        if (i < nrow-1 && map[i+1][j] == map[i][j]) {
            // bottom neighbor
            nextNodes.emplace(i+2, j+1);
        }
    }
    return reachableMap[q.from.first-1][q.from.second-1] == reachableMap[q.to.first-1][q.to.second-1];
}


Answer Graph::search(const Query& q) {
    assert(m_nodes.find(q.from) != m_nodes.end() && "source node invalid!");
    assert(m_nodes.find(q.to) != m_nodes.end() && "target node invalid!");
    Node& sourceNode = m_nodes.at(q.from);
    Node& targetNode = m_nodes.at(q.to);

    Answer answer = Answer::NEITHER;
    if (sourceNode.mapValue == targetNode.mapValue) {
        // possible
        bool reachedGoal = bfs(sourceNode, targetNode);
        if (reachedGoal && sourceNode.mapValue == 0) {
            answer = Answer::BINARY;
        } else if (reachedGoal && sourceNode.mapValue == 1) {
            answer = Answer::DECIMAL;
        }
    }
    // reset state of graph before returning
    resetState();
    return(answer);
}

void Graph::buildFromMap(const std::vector<std::vector<int>>& map, size_t nrow, size_t ncol) {
    auto start = std::chrono::steady_clock::now();
    // std::cout << "nrow: " << nrow << ", ncol: " << ncol << std::endl;
    // create nodes
    for (size_t i = 0; i < nrow; ++i) {
        for (size_t j = 0; j < ncol; ++j) {
            // std::cout << "i: " << i << ", j: " << j << std::endl << std::flush;
            Node n;
            n.id = std::make_pair(i+1, j+1); 
            n.status = Status::NOT_VISITED;
            n.mapValue = map[i][j];
            // store neighbors
            if (j > 0) {
                // left neighbor
                n.addNeighbor(&m_nodes[std::make_pair(i+1, j)], map[i][j-1]);
            }
            if (i > 0) {
                // top neighbor
                n.addNeighbor(&m_nodes[std::make_pair(i, j+1)], map[i-1][j]);
            }
            if (j < ncol-1) {
                // right neighbor
                n.addNeighbor(&m_nodes[std::make_pair(i+1, j+2)], map[i][j+1]);
            }
            if (i < nrow-1) {
                // bottom neighbor
                n.addNeighbor(&m_nodes[std::make_pair(i+2, j+1)], map[i+1][j]);
            }
            m_nodes[n.id] = n;
        }
    }
    auto end = std::chrono::steady_clock::now();
    #if DEBUG
    std::cout << "Graph Generation | Elapsed time in milliseconds : " 
    << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    << " ms" << std::endl;
    #endif
}


class Parser {
    public:
        Parser();
        Parser(std::string sampleFile) : m_sampleFile(sampleFile) {}
        void parse();
            /// parse file or stdin
        Graph& getGraph() { return m_graph; }
            /// a graph representing the parsed map
        size_t getNumberOfQueries() { return m_queries.size(); }
            /// the total number of queries in parsed sample file
        Query getQuery(size_t idx) { return m_queries[idx]; }
            /// query: two pairs of x,y coordiates: search for route {from} {to}
        const std::vector<std::vector<int>>& getMap() { return m_map; }
        size_t getRows() { return m_nrow; }
        size_t getCols() { return m_ncol; }
    private:
        void parseFile();
            /// parse the loaded sample file
        void parseStdIn();
            /// parse from stdin
        Graph m_graph;
            // Graph structure representing the parsed map
        std::vector<Query> m_queries;
            // Queries to be checked for possible routes
        std::string m_sampleFile;
            // file to be parsed
        std::vector<std::vector<int>> m_map;
            // the parsed map
        size_t m_nrow;
            // nbr of map rows
        size_t m_ncol;
            // nbr of map columns
};

Parser::Parser() = default;

void Parser::parse() {
    auto start = std::chrono::steady_clock::now();
    if (m_sampleFile == "") {
        parseStdIn();
    } else {
        parseFile();
    }
    auto end = std::chrono::steady_clock::now();
    #if DEBUG
    std::cout << "Parsing | Elapsed time in milliseconds : " 
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
		<< " ms" << std::endl;
    #endif
}
void Parser::parseStdIn() {
    std::cin >> m_nrow >> m_ncol;
    m_map = std::vector<std::vector<int>>(m_nrow, std::vector<int>(m_ncol));
    size_t nbrQueries;
    std::vector<Query> queries;
    
    for (int i = 0; i < m_nrow; i++) {
		for (int j = 0; j < m_ncol; j++) {
			char b;
            std::cin >> b;
            m_map[i][j] = b - 48; // char rep to int value
		}
	}
    // read no of queries
    int nQueries;
    std::cin >> nQueries;
    // read queries
    while (nQueries--) {
        Query q;
        std::string x1, y1, x2, y2;
        std::cin >> x1 >> y1 >> x2 >> y2;
        std::string line = x1 + " " + y1 + " " + x2 + " " + y2;
        //std::cout << "read query line: " << line << std::endl;
        std::istringstream iss(line);
        iss >> q;
        queries.push_back(q);
    }
    assert(m_map.size() == m_nrow);
    // store data that is needed for the solver
    m_queries = queries;
}

void Parser::parseFile() {
    std::ifstream fs;
    fs.open(m_sampleFile);
    std::string line;
    size_t lineNbr = 0;

    // variables for data to be parsed
    size_t nbrQueries;
    std::vector<Query> queries;
    
    while (std::getline(fs, line)) {
        if (lineNbr == 0) {
            // read nrow, ncol
            std::istringstream iss(line);
            iss >> m_nrow >> m_ncol;
            m_map.resize(m_nrow);
        } else if (lineNbr > 0 && lineNbr <= m_nrow) {
            // read map
            std::vector<int> row(m_ncol);
            for (size_t i = 0; i < m_ncol; ++i) {
                row[i] = line[i] - 48; // char rep to int value
            }
            assert(row.size() == m_ncol);
            m_map[lineNbr-1] = row;
        } else if (lineNbr == (m_nrow + 1)) {
            // read nbr of queries
            std::istringstream iss(line);
            iss >> nbrQueries;
        } else {
            // read queries
            std::istringstream iss(line);
            Query q;
            iss >> q;
            queries.push_back(q);
        }
        ++lineNbr;
    }
    fs.close();
    assert(lineNbr != 0 && "Could not read anything from file");
    assert(queries.size() == nbrQueries);
    assert(m_map.size() == m_nrow);
    // store data that is needed for the solver
    m_queries = queries;
}

/// breadth-first search without building the graph first
Answer quickSearch(const Query& q, const std::vector<std::vector<int>>& map) {
    Answer answer = Answer::NEITHER;
    int sourceValue = map[q.from.first-1][q.from.second-1];
    if (sourceValue == map[q.to.first-1][q.to.second-1]) {
        // possible
        bool reachedGoal = bfsInPlace(q, map);
        if (reachedGoal && sourceValue == 0) {
            answer = Answer::BINARY;
        } else if (reachedGoal && sourceValue == 1) {
            answer = Answer::DECIMAL;
        }
    }
    return(answer);
}

/// breadth-first search without building the graph first
/// and using knowledge from previous iterations
Answer quickerSearch(const Query& q, const std::vector<std::vector<int>>& map,
                     std::vector<std::vector<int>>& reachableMap, int runNbr) {
    // reachableMap represents equivalence classes of reachability
    // all nodes that can reach each other are assigned the same integer
    // a value of 0 means: no statement possible (not evaluated / not reachable)
    Answer answer = Answer::NEITHER;

    int sx = q.from.first-1;
    int sy = q.from.second-1;
    int tx = q.to.first-1;
    int ty = q.to.second-1;

    int sourceValue = map[sx][sy];
    int targetValue = map[tx][ty];


    if (reachableMap[sx][sy] != 0 && reachableMap[sx][sy] == reachableMap[tx][ty]) {
        // case 1: we know that source -> target has a route
        answer = map[sx][sy] == 0 ? Answer::BINARY : Answer::DECIMAL;
    } else if (reachableMap[sx][sy] != reachableMap[tx][ty]) {
        // case 2: source and target are in different reachability equivalence classes
        answer = Answer::NEITHER;
    } else if (sourceValue == targetValue) {
        // case 3: there could be a route but we still have to check
        bool reachedGoal = bfsInPlaceMem(q, map, reachableMap, ++runNbr);
        if (reachedGoal && sourceValue == 0) {
            answer = Answer::BINARY;
        } else if (reachedGoal && sourceValue == 1) {
            answer = Answer::DECIMAL;
        }
    } else {
        // case 4: sourceValue != targetValue -> impossible
        answer = Answer::NEITHER;
    }
    return(answer);
}

}


/* Solver */

Solver::Solver(std::string sampleFile) : m_sampleFile(sampleFile) {
}


std::vector<Answer> Solver::solve() {
    Parser parser(m_sampleFile);
    parser.parse();
    Graph g;
    bool prebuildGraph = false; // 3768 ms w/ prebuild vs 43 ms w/o prebuild
    if (prebuildGraph) {
        g.buildFromMap(parser.getMap(), parser.getRows(), parser.getCols());
    }
    size_t nQueries = parser.getNumberOfQueries();
    std::vector<Answer> answers(nQueries);
    std::vector<std::vector<int>> reachableMap = std::vector<std::vector<int>>(parser.getRows(), std::vector<int>(parser.getCols()));

    for (size_t i = 0; i < nQueries; ++i) {
        auto t = std::chrono::steady_clock::now();
        Answer a;
        if (prebuildGraph) {
            a = g.search( parser.getQuery(i) ); // slow search requires that buildFromMap was called
        } else {
            //a = quickSearch( parser.getQuery(i), parser.getMap() ); // fast search: no build from map necessary
            a = quickerSearch( parser.getQuery(i), parser.getMap(), reachableMap, i); // faster: use info from previous runs about reachability
        }
        answers[i] = a;
        auto tt = std::chrono::steady_clock::now();
        #if DEBUG
            std::cout << "Query " << i << " | Elapsed time in milliseconds : " 
            << std::chrono::duration_cast<std::chrono::milliseconds>(tt - t).count()
            << " ms" << std::endl;
            // TODO: to be below < 1 s for 1000 queries, we need to require only 1 ms per query ...
        #endif
    }
    return answers;
}

