#include "ChessPiece.h"
#include "ChessMoves.h"
#include "ChessBoard.h"

//Constructor
ChessPiece::ChessPiece(Color c) : c(c){}

//Destructor
ChessPiece::~ChessPiece(){}
//getter
Color ChessPiece::getColor() const{
    return c;
}

//Derived Classes

//Pawn implementation
//Virtual fnc return P for White small p for black
Pawn::Pawn(Color c) : ChessPiece(c){}
char Pawn::getSymbol() const{
    return (c == Color::White ? 'P': 'p');
}

//Pawn logic for move validation\

bool Pawn::isValidMove(const ChessMoves& move,const ChessBoard& board)const {

    //To ensure forward direction of pawns
    //maybe need to add more after will check logic...
    int direction = (c== Color::White ? -1 : 1);
    int startingRow = (c== Color::White ? 6 : 1);

    //Row diff initialization to check for movements of pawns
    int rowDifference= move.desiredRow-move.initialRow;
    int colDifference= abs(move.desiredCol-move.initialCol);

    //Pawn moves 1 square
    if (colDifference == 0 && rowDifference==direction&&board.getPiece(move.desiredRow,move.desiredCol)==nullptr)
        return true;

    //Pawn moves 2 steps
    if (colDifference == 0 && rowDifference == 2 * direction && move.initialRow == startingRow &&
        board.getPiece(move.initialRow + direction, move.initialCol) == nullptr &&
        board.getPiece(move.desiredRow, move.desiredCol) == nullptr)
        return true;

    //Pawn goes diagonal from a piece
    if (colDifference == 1 && rowDifference == direction) {
        auto& target = board.getPiece(move.desiredRow, move.desiredCol);
        if (target && target->getColor() != c)
            return true;
    }
    return false;
}


//Rook implementation
Rook::Rook(Color c) : ChessPiece(c) { }
char Rook::getSymbol() const {
    return (c == Color::White) ? 'R' : 'r';
}

//Rook logic move validation

bool Rook::isValidMove(const ChessMoves& move,const ChessBoard& board)const {
    int rowDifference = move.desiredRow - move.initialRow;
    int colDifference = move.desiredCol - move.initialCol;

    //This is to prevent from diagonal movements if it moves on same row diff is 0 same for col.
    if (rowDifference != 0 && colDifference != 0)
        return false;

    //Step directions used to validate pathing if theres a piece to caputure or be blocked by.
    int stepRow = (rowDifference == 0) ? 0 : (rowDifference > 0 ? 1 : -1);
    int stepCol = (colDifference == 0) ? 0 : (colDifference > 0 ? 1 : -1);

    int currentRow = move.initialRow + stepRow;
    int currentCol = move.initialCol + stepCol;

    //This while loop can be used to check for pieces block the path
    while (currentRow != move.desiredRow || currentCol != move.desiredCol) {
        //if false then blocked.
        if (board.getPiece(currentRow, currentCol) != nullptr)
            return false;
        currentRow += stepRow;
        currentCol += stepCol;
    }
    //Verifiy Destination squares
    auto& target = board.getPiece(move.desiredRow, move.desiredCol);
    //The square empty
    if (target == nullptr)
        return true;

    //This is for caputuring depending on the color
    return target->getColor() != c;
}


//Knight implementation
Knight::Knight(Color c) : ChessPiece(c) { }
char Knight::getSymbol() const {
    return (c == Color::White) ? 'N' : 'n';
}

//Knight move validation

bool Knight::isValidMove(const ChessMoves& move, const ChessBoard& board) const {
    int rowDifference = abs(move.desiredRow - move.initialRow);
    int colDifference = abs(move.desiredCol - move.initialCol);

    //Moving L wise check
    if (!((rowDifference == 2 && colDifference == 1) || (rowDifference == 1 && colDifference == 2)))
        return false;

    //Verifiy Destination squares
    auto& target = board.getPiece(move.desiredRow, move.desiredCol);
    //The square empty
    if (target == nullptr)
        return true;

    //This is for caputuring depending on the color
    return target->getColor() != c;
}

//Bishop implementation
Bishop::Bishop(Color c) : ChessPiece(c) { }
char Bishop::getSymbol() const {
    return (c == Color::White) ? 'B' : 'b';
}

//Bishop implementation for validation of moves

bool Bishop::isValidMove(const ChessMoves& move, const ChessBoard& board) const {
    int rowDifference = move.desiredRow - move.initialRow;
    int colDifference = move.desiredCol - move.initialCol;

    //Move diagonal-> absolute row and col diff are equal
    if (abs(rowDifference) != abs(colDifference))
        return false;

    //for the direction of bishop on board
    int stepRow = (rowDifference > 0) ? 1 : -1;
    int stepCol = (colDifference > 0) ? 1 : -1;

    int currentRow = move.initialRow + stepRow;
    int currentCol = move.initialCol + stepCol;

    //This while loop can be used to check for pieces block the path
    while (currentRow != move.desiredRow || currentCol != move.desiredCol) {
        //if false then blocked.
        if (board.getPiece(currentRow, currentCol) != nullptr)
            return false;
        currentRow += stepRow;
        currentCol += stepCol;
    }

    //Verifiy Destination squares
    auto& target = board.getPiece(move.desiredRow, move.desiredCol);
    //The square empty
    if (target == nullptr)
        return true;

    //This is for caputuring depending on the color
    return target->getColor() != c;
}


//Queen implementation
Queen::Queen(Color c) : ChessPiece(c) { }
char Queen::getSymbol() const {
    return (c == Color::White) ? 'Q' : 'q';
}


//Queen move validations check

bool Queen::isValidMove(const ChessMoves& move, const ChessBoard& board) const {

    //initialize row diff
    int rowDifference = move.desiredRow - move.initialRow;
    int colDifference = move.desiredCol - move.initialCol;

    //abs to make all diagonals valid
    bool isDiagonal = (abs(rowDifference) == abs(colDifference));
    //just directional check
    bool isStraight = (rowDifference == 0 || colDifference == 0);
    //in case illegal move like L shape for queen
    if (!isDiagonal && !isStraight)
        return false;

    int stepRow = (rowDifference == 0) ? 0 : (rowDifference > 0 ? 1 : -1);
    int stepCol = (colDifference == 0) ? 0 : (colDifference > 0 ? 1 : -1);

    //initialize current row and col for check path after
    int currentRow = move.initialRow + stepRow;
    int currentCol = move.initialCol + stepCol;

    //This while loop can be used to check for pieces block the path
    while (currentRow != move.desiredRow || currentCol != move.desiredCol) {
        //if false then blocked.
        if (board.getPiece(currentRow, currentCol) != nullptr)
            return false;
        currentRow += stepRow;
        currentCol += stepCol;
    }

    //Verify Destination squares
    auto& target = board.getPiece(move.desiredRow, move.desiredCol);
    //The square empty
    if (target == nullptr)
        return true;

    //This is for capturing depending on the color
    return target->getColor() != c;
}



//King implementation
King::King(Color c) : ChessPiece(c) { }
char King::getSymbol() const {
    return (c == Color::White) ? 'K' : 'k';
}

//King move checkers
bool King::isValidMove(const ChessMoves& move, const ChessBoard& board) const {
    //for direction king will move ignoring direction
    int rowDifference = abs(move.desiredRow - move.initialRow);
    int colDifference = abs(move.desiredCol - move.initialCol);

    //limit king movements only 1 square
    if (rowDifference > 1 || colDifference > 1)
        return false;

    auto& target = board.getPiece(move.desiredRow, move.desiredCol);
    if (target == nullptr)
        return true;

    return target->getColor() != c;
}


//Clone for pieces
std::unique_ptr<ChessPiece> Pawn::clone() const {
    return std::make_unique<Pawn>(*this);
}


std::unique_ptr<ChessPiece> Rook::clone() const {
    return std::make_unique<Rook>(*this);
}

std::unique_ptr<ChessPiece> Knight::clone() const {
    return std::make_unique<Knight>(*this);
}

std::unique_ptr<ChessPiece> Bishop::clone() const {
    return std::make_unique<Bishop>(*this);
}

std::unique_ptr<ChessPiece> Queen::clone() const {
    return std::make_unique<Queen>(*this);
}

std::unique_ptr<ChessPiece> King::clone() const {
    return std::make_unique<King>(*this);
}
