#ifndef CHESSPIECE_H
#define CHESSPIECE_H
#include <string>
#include <memory>


//Forward declarations
class ChessMoves;
class ChessBoard;

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

    //Pvf for checking move validity for every single piece
    virtual bool isValidMove(const ChessMoves& move, const ChessBoard& board) const = 0;
    //Virtual Clone method to dup the unique ids of pieces
    virtual std::unique_ptr<ChessPiece> clone() const = 0;

protected:
    Color c;
};

class Pawn : public ChessPiece{
public:
    Pawn(Color c);
    char getSymbol() const override;
    bool isValidMove(const ChessMoves& move, const ChessBoard& board) const override;
    std::unique_ptr<ChessPiece> clone() const override;
};

class Rook : public ChessPiece{
public:
    Rook(Color c);
    char getSymbol() const override;
    bool isValidMove(const ChessMoves& move, const ChessBoard& board) const override;
    std::unique_ptr<ChessPiece> clone() const override;
};

class Knight : public ChessPiece{
public:
    Knight(Color c);
    char getSymbol() const override;
    bool isValidMove(const ChessMoves& move, const ChessBoard& board) const override;
    std::unique_ptr<ChessPiece> clone() const override;
};

class Bishop : public ChessPiece{
public:
    Bishop(Color c);
    char getSymbol() const override;
    bool isValidMove(const ChessMoves& move, const ChessBoard& board) const override;
    std::unique_ptr<ChessPiece> clone() const override;
};

class Queen : public ChessPiece{
public:
    Queen(Color c);
    char getSymbol() const override;
    bool isValidMove(const ChessMoves& move, const ChessBoard& board) const override;
    std::unique_ptr<ChessPiece> clone() const override;
};

class King : public ChessPiece{
public:
    King(Color c);
    char getSymbol() const override;
    bool isValidMove(const ChessMoves& move, const ChessBoard& board) const override;
    std::unique_ptr<ChessPiece> clone() const override;
};




#endif 