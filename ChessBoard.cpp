#include "ChessBoard.h"
#include <iostream>

ChessBoard::ChessBoard() {
    board.resize(BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        board[i].resize(BOARD_SIZE);
    }
    initializeBoard();
}

void ChessBoard::initializeBoard(){
    //Positioning for black piece
    //Creating unique pointer to point to each piece object and assigning to initial position.
    board[0][0] = std::make_unique<Rook>(Color::Black);
    board[0][1] = std::make_unique<Knight>(Color::Black);
    board[0][2] = std::make_unique<Bishop>(Color::Black);
    board[0][3] = std::make_unique<Queen>(Color::Black);
    board[0][4] = std::make_unique<King>(Color::Black);
    board[0][5] = std::make_unique<Bishop>(Color::Black);
    board[0][6] = std::make_unique<Knight>(Color::Black);
    board[0][7] = std::make_unique<Rook>(Color::Black);
    //Looping for all pawns
    for (int j = 0; j < BOARD_SIZE; j++) {
        board[1][j] = std::make_unique<Pawn>(Color::Black);
    }
    
    //Positioning for white pieces.
    board[7][0] = std::make_unique<Rook>(Color::White);
    board[7][1] = std::make_unique<Knight>(Color::White);
    board[7][2] = std::make_unique<Bishop>(Color::White);
    board[7][3] = std::make_unique<Queen>(Color::White);
    board[7][4] = std::make_unique<King>(Color::White);
    board[7][5] = std::make_unique<Bishop>(Color::White);
    board[7][6] = std::make_unique<Knight>(Color::White);
    board[7][7] = std::make_unique<Rook>(Color::White);
    for (int j = 0; j < BOARD_SIZE; j++) {
        board[6][j] = std::make_unique<Pawn>(Color::White);
    }

}

void ChessBoard::printBoard() const {
    //Loop through the board rows.
    for (int i = 0; i < BOARD_SIZE; i++) {
        //Loop through the board columns.
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j])
                std::cout << board[i][j]->getSymbol() << " ";
            else
                std::cout << "X ";
        }
        std::cout << std::endl;
    }
}
//function to return piece
std::unique_ptr<ChessPiece>& ChessBoard::getPiece(int row, int col) {
    return board[row][col];
}
//function to return piece in const board.
const std::unique_ptr<ChessPiece>& ChessBoard::getPiece(int row, int col) const {
    return board[row][col];
}