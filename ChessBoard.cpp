#include "ChessBoard.h"
#include "ChessMoves.h"
#include <iostream>
#include "ChessGameEngine.h"
#include <cmath>

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

//Copy Constructor
ChessBoard::ChessBoard(const ChessBoard& other) {
    board.resize(BOARD_SIZE);
    for (int row = 0; row < BOARD_SIZE; ++row) {
        board[row].resize(BOARD_SIZE);
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (other.board[row][col]) {
                board[row][col] = other.board[row][col]->clone();
            }
        }
    }
}

//Assignment operator
ChessBoard& ChessBoard::operator=(const ChessBoard& other) {
    if (this == &other) return *this; // self-assignment check

    //Clear current board
    for (auto& row : board) {
        for (auto& piecePtr : row) {
            piecePtr.reset();
        }
    }

    //Resize board
    board.resize(BOARD_SIZE);
    for (auto& row : board)
        row.resize(BOARD_SIZE);

    //Deep copy pieces from other board
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (other.board[r][c]) {
                board[r][c] = other.board[r][c]->clone();
            } else {
                board[r][c] = nullptr;
            }
        }
    }
    return *this;
}

//Making sure pathing is good for castle
bool ChessBoard::isPathClearForCastling(Color color, bool kingside) const {
    int row = (color == Color::White) ? 7 : 0;

    int kingCol = 4;
    int rookCol = kingside ? 7 : 0;

    int direction = (rookCol - kingCol) > 0 ? 1 : -1;

    //Check if squares between king and rook are empty
    for (int col = kingCol + direction; col != rookCol; col += direction) {
        if (getPiece(row, col))
            return false;
    }

    //Check that squares the king crosses are not under attack
    for (int step = 0; step <= 2; ++step) {
        int testCol = kingCol + step * direction;
        ChessMoves checkMove(row, testCol, row, testCol);  // Dummy move from and to same square
        if (isSquareUnderAttack(row, testCol, color))
            return false;

    }

    return true;
}

//Checking if current position of square is hit by any piece
bool ChessBoard::isSquareUnderAttack(int row, int col, Color threatenedColor) const {
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            const auto& piece = getPiece(r, c);
            if (piece && piece->getColor() != threatenedColor) {
                ChessMoves testMove(r, c, row, col);
                if (piece->isValidMove(testMove, *this)) {
                    return true;
                }
            }
        }
    }
    return false;
}
