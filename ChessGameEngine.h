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

private:
    ChessBoard board;
    Color currentTurn;

    //Move validation
    bool isMoveLegal(const ChessMoves& move) const;

    //Changing turns
    void switchTurn();
};

#endif
