#include <iostream>
#include <vector>
#include <optional>
#include <set>

int DIM = 4; 
using Board = std::vector<std::vector<int>>;
using MSet = std::set<std::pair<int,int>>; // collection of merged tiles

enum class Move {
    LEFT = 0,
    UP, 
    RIGHT,
    DOWN
};

void parseInput(Board& board, Move& move) {
    int gameDim = DIM; // 4 by 4 game board
    while (gameDim--) {
        int a, b, c, d;
        std::cin >> a >> b >> c >> d;
        std::vector<int> row = {a, b, c, d};
        board.push_back(row);
    }

    // read move:
    int m;
    std::cin >> m;
    move = static_cast<Move>(m);
}

bool cellEmpty(const Board& board, int i, int j) {
    return board[i][j] == 0;
}

void swap(int& v1, int& v2) {
   int c = v1;
   v1 = v2;
   v2 = c;
}

void move(Board& board, const Move& m, int i, int j, MSet& mergedTiles);

void moveTo(Board& board, int i, int j, int ni, int nj, const Move& m, MSet& mergedTiles) {
    if (ni >= 0 && ni < DIM && nj >= 0 && nj < DIM) {
       if (cellEmpty(board, ni, nj)) {
            swap(board[i][j], board[ni][nj]);
            move(board, m, ni, nj, mergedTiles); // could move again
       } else { // collision
            if (board[i][j] == board[ni][nj] && mergedTiles.find(std::make_pair(ni,nj)) == mergedTiles.end()) {
                // same value that should be merged because this tile hasn't merged yet
                board[ni][nj] = 2 * board[i][j];
                board[i][j] = 0;
                mergedTiles.insert(std::make_pair(ni,nj));
            } 
            return;
       }
    }
}


void move(Board& board, const Move& m, int i, int j, MSet& mergedTiles) {
    if (board[i][j] == 0) {
        // empty tiles cant move
        return;
    }
    int ni = i; // proposed coordinates from move
    int nj = j; 
    switch(m) {
        case Move::LEFT:
            nj -= 1;
            break;
        case Move::RIGHT:
            nj += 1;
            break;
        case Move::UP:
            ni -= 1;
            break;
        case Move::DOWN:
            ni += 1;
            break;
    }
    moveTo(board, i, j, ni, nj, m, mergedTiles);
}

void solve(Board& board, const Move& m) {
    int empty = 0; // value of 0: empty field in board
    MSet mergedTiles; // indices of tiles that have been merged due to the move and shouldnt merge again
    if (m == Move::LEFT || m == Move::UP) {
        // start with entries at top & LHS
        for (int i = 0; i < DIM; ++i) {
            for (int j = 0; j < DIM; ++j) {
                move(board, m, i, j, mergedTiles);
            }
        }
    } else if (m == Move::RIGHT) {
        // start with entries at RHS
        for (int i = 0; i < DIM; ++i) {
            for (int j = DIM-1; j >= 0; --j) {
                move(board, m, i, j, mergedTiles);
            }
        }
    } else if (m == Move::DOWN) {
        // start with entries at bottom
        for (int i = DIM-1; i >= 0; --i) {
            for (int j = 0; j < DIM; ++j) {
                move(board, m, i, j, mergedTiles);
            }
        }
    }
}

void printOutput(const Board& board) {
    for (int i = 0; i < DIM; ++i) {
        for (int j = 0; j < DIM; ++j) {
            std::cout << board[i][j];
            if (j != DIM-1) {
                std::cout << " ";
            } else {
                std::cout << std::endl;
            }
        }
    }
}
int main(int argc, char** argv) {
    Board board;
    Move move;
    parseInput(board, move);

    // determine state after performing move on board
    solve(board, move); 
    printOutput(board);
}
