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
    //Retrieving the piece
    std::unique_ptr<ChessPiece>& getPiece(int row, int col);
    //Retrieving pieces in a constant board... fix after.
    const std::unique_ptr<ChessPiece>& getPiece(int row, int col) const;


private: 
    //2d grid for board
    //smart pointers for chess piece manangements
    std::vector<std::vector<std::unique_ptr<ChessPiece>>> board;

    //inital board
    void initializeBoard();

};

#endif