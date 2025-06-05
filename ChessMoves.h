#ifndef CHESSMOVES_H
#define CHESSMOVES_H
#include <cmath>

class ChessMoves {
public:
    //Labeling variables for the positioning of the pieces
    int initialRow;
    int initialCol;
    int desiredRow;
    int desiredCol;

    //Boolean to handle captured pieces and promoted pieces
    bool isCaptured=false;
    bool isPromotion=false;
    //Defaulted to Queen for now change later with user input
    char promotionPiece = 'Q';

    //Constructor
    ChessMoves(int initialRow, int initialCol, int desiredRow, int desiredCol)
        : initialRow(initialRow), initialCol(initialCol),desiredRow(desiredRow), desiredCol(desiredCol) {}
    //Constructor with all maybe change later.
    ChessMoves(int initialRow, int initialCol, int desiredRow, int desiredCol,bool isCaptured=false,bool isPromotion=false,char promotionPiece='')
        : initialRow(initialRow), initialCol(initialCol),desiredRow(desiredRow), desiredCol(desiredCol),
        isCaptured(isCaptured), isPromotion(isPromotion),promotionPiece(promotionPiece) {}
    //using abs for diagonal movements
    bool isDiagonal() const {
        return abs(initialRow-desiredRow)==abs(initialCol-desiredCol);
    }

    bool isStraight() const {
        return (initialRow==desiredRow || initialCol==desiredCol);
    }

#endif
};