#include "ChessPiece.h"

//Constructor
ChessPiece::ChessPiece(Color c) : c(c){}

//Destructor
ChessPiece::~ChessPiece(){}
//getter
Color ChessPiece::getColor() const{
    return c;
}


//Derived Classes

// Pawn implementation
//Virtual fnc return P for White small p for black
Pawn::Pawn(Color c) : ChessPiece(c){}
char Pawn::getSymbol() const{
    return (c == Color::White ? 'P': 'p');
}

// Rook implementation
Rook::Rook(Color c) : ChessPiece(c) { }
char Rook::getSymbol() const {
    return (c == Color::White) ? 'R' : 'r';
}

// Knight implementation
Knight::Knight(Color c) : ChessPiece(c) { }
char Knight::getSymbol() const {
    return (c == Color::White) ? 'N' : 'n';
}

// Bishop implementation
Bishop::Bishop(Color c) : ChessPiece(c) { }
char Bishop::getSymbol() const {
    return (c == Color::White) ? 'B' : 'b';
}

// Queen implementation
Queen::Queen(Color c) : ChessPiece(c) { }
char Queen::getSymbol() const {
    return (c == Color::White) ? 'Q' : 'q';
}

// King implementation
King::King(Color c) : ChessPiece(c) { }
char King::getSymbol() const {
    return (c == Color::White) ? 'K' : 'k';
}