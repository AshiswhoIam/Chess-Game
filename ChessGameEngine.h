#ifndef CHESSGAMEENGINE_H
#define CHESSGAMEENGINE_H

#include "ChessBoard.h"
#include "ChessMoves.h"

class ChessGameEngine {
public:
    ChessGameEngine();

    //Making a move
    bool makeMove(const ChessMoves& move);

    //Displaying the game
    void displayGame() const;

    //Checking if King in check
    bool isKingInCheck(Color kingColor) const;

    //To Check if Kingis in Checkmate or has more moves
    bool hasLegalMoves(Color playerColor) const;
    //check if checkmate
    bool isCheckmated(Color playerColor) const;
    //check for stalemate
    bool isStalemated(Color playerColor) const;


private:
    ChessBoard board;
    Color currentTurn;

    //Move validation
    bool isMoveLegal(const ChessMoves& move) const;

    //Changing turns
    void switchTurn();
};

#endif
