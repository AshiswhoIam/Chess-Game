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

    //To Check if King is in Checkmate or has more moves
    bool hasLegalMoves(Color playerColor) const;
    //check if checkmate
    bool isCheckmated(Color playerColor) const;
    //check for stalemate
    bool isStalemated(Color playerColor) const;
    //getting color turn
    Color getCurrentTurn() const;
    //For castling check
    bool canCastle(Color color, bool kingside) const;
    //Performing Castling
    void performCastling(Color color, bool kingside);
    //Checking if the square is hit by enemy for castling
    bool isSquareUnderAttack(int row, int col, Color threatenedColor) const;
    //getter for board
    const ChessBoard& getBoard() const;
    //Handling promotion on screen
    bool isPromotionPending() const;
    //To do the promotion on selection
    void executePromotion(char promotionPiece);
    //Move validation
    bool isMoveLegal(const ChessMoves& move) const;
private:
    ChessBoard board;
    Color currentTurn;



    //Changing turns
    void switchTurn();

    //Track king and rook movement for castling
    bool whiteKingMoved = false;
    bool blackKingMoved = false;
    bool whiteRookKingsideMoved = false;
    bool whiteRookQueensideMoved = false;
    bool blackRookKingsideMoved = false;
    bool blackRookQueensideMoved = false;
    //Chess move on a temporary copy of board and then checks king.
    bool isKingInCheckAfterMove(const ChessMoves& move, Color kingColor) const;
    //For proper promotion in SFML
    bool promotionPending = false;
    int promotionRow = -1, promotionCol = -1;
    Color promotionColor = Color::White;

};

#endif
