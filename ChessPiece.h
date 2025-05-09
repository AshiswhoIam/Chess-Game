#ifndef CHESSPIECE_H
#define CHESSPIECE_H
#include <string>

enum class Color{
White,
Black
};

//main chess piece class

class ChessPiece {
public:
    //Constructor
    ChessPiece(Color c);
    //Destructor
    virtual ~ChessPiece();
    //Getter color
    Color getColor() const;
    //Pure Virt fnc
    virtual char getSymbol() const=0;

protected:
    Color c;
};

class Pawn : public ChessPiece{
public:
    Pawn(Color c);
    char getSymbol() const override;
};

class Rook : public ChessPiece{
public:
    Rook(Color c);
    char getSymbol() const override;
};

class Knight : public ChessPiece{
public:
    Knight(Color c);
    char getSymbol() const override;
};

class Bishop : public ChessPiece{
public:
    Bishop(Color c);
    char getSymbol() const override;
};

class Queen : public ChessPiece{
public:
    Queen(Color c);
    char getSymbol() const override;
};

class King : public ChessPiece{
public:
    King(Color c);
    char getSymbol() const override;
};




#endif 