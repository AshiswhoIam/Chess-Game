#include "ChessMoves.h"

//Minimal Constructor.
ChessMoves::ChessMoves(int initialRow, int initialCol, int desiredRow, int desiredCol)
    : initialRow(initialRow), initialCol(initialCol),desiredRow(desiredRow), desiredCol(desiredCol) {}

//Constructor with all
ChessMoves::ChessMoves(int initialRow, int initialCol, int desiredRow, int desiredCol,
                       bool isCaptured, bool isPromotion, char promotionPiece)
    : initialRow(initialRow), initialCol(initialCol),desiredRow(desiredRow), desiredCol(desiredCol),
      isCaptured(isCaptured), isPromotion(isPromotion),promotionPiece(promotionPiece) {}

//Checking for diagonal movement
bool ChessMoves::isDiagonal() const {
    return std::abs(initialRow - desiredRow) == std::abs(initialCol - desiredCol);
}

//Checking for straight move
bool ChessMoves::isStraight() const {
    return (initialRow == desiredRow || initialCol == desiredCol);
}
