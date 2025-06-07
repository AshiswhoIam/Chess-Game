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
    //Copy constructor
    ChessBoard(const ChessBoard& other);

    //Board State
    void printBoard() const;
    //Retrieving the piece
    std::unique_ptr<ChessPiece>& getPiece(int row, int col);
    //Retrieving pieces in a constant board... fix after.
    const std::unique_ptr<ChessPiece>& getPiece(int row, int col) const;
    //Assignment operator
    ChessBoard& operator=(const ChessBoard& other);



private: 
    //2d grid for board
    //smart pointers for chess piece managements
    std::vector<std::vector<std::unique_ptr<ChessPiece>>> board;

    //initial board
    void initializeBoard();

};

#endif