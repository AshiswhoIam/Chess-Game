#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <memory>
#include "ChessPiece.h"

//Defining the board size
const int BOARD_SIZE = 8;

class ChessBoard{
public:
    //Constructor
    ChessBoard();

    //Board State
    void printBoard() const;

private: 
    //2d grid for board
    //smart pointers for chess piece manangements
    std::vector<std::vector<std::unique_ptr<ChessPiece>>> board;

    //inital board
    void initializeBoard();

};

#endif