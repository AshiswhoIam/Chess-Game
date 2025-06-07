#include "ChessGameEngine.h"
#include <iostream>
#include <cctype>

//Constructor to initialize board with pieces
ChessGameEngine::ChessGameEngine() : currentTurn(Color::White) {

}

//Validation of the moves done
bool ChessGameEngine::makeMove(const ChessMoves& move) {
    if (!isMoveLegal(move)) {
        std::cout << "Illegal move has been made!" << std::endl;
        return false;
    }

    //Get the piece at the source/ini position
    auto& sourcePiecePtr = board.getPiece(move.initialRow, move.initialCol);
    if (!sourcePiecePtr) {
        std::cout << "No piece at the source position." << std::endl;
        return false;
    }

    //Get the piece at the destination
    auto& destPiecePtr = board.getPiece(move.desiredRow, move.desiredCol);

    //Capturing if the dest piece is not of same color
    if (destPiecePtr && destPiecePtr->getColor() != sourcePiecePtr->getColor()) {
        //Remove the dest piece that is captured
        destPiecePtr.reset();
    }

    //Move the piece here transfering unique ptr
    destPiecePtr = std::move(sourcePiecePtr);

    //The source/ini square is now empty
    sourcePiecePtr = nullptr;

    //This ill do for promotion handling replacing the pawn with piece chosen
    if (destPiecePtr) {
        //dest piece is smart pointer getting dereferenced for symbol checking for pawn
        char symbol = destPiecePtr->getSymbol();
        //this is for getting the color of the piece
        Color pieceColor = destPiecePtr->getColor();
        //Set the promotion row to 0 for White pawns and 7 for Black pawns.
        int promotionRow = (pieceColor == Color::White) ? 0 : 7;
        //myb add fnc later for creating piece with sybol char
        if ((symbol == 'P' || symbol == 'p') && move.desiredRow == promotionRow) {
            char promo;
            std::cout << "Pawn promotion! Choose piece (Q, R, B, N): ";
            std::cin >> promo;
            //Turning char into upcase
            promo = std::toupper(promo);


            switch (promo) {
                case 'Q':
                    destPiecePtr = std::make_unique<Queen>(pieceColor);
                    break;
                case 'R':
                    destPiecePtr = std::make_unique<Rook>(pieceColor);
                    break;
                case 'B':
                    destPiecePtr = std::make_unique<Bishop>(pieceColor);
                    break;
                case 'N':
                    destPiecePtr = std::make_unique<Knight>(pieceColor);
                    break;
                default:
                    //Default to Queen if unknown
                    std::cout << "Invalid input. Promoting to Queen by default.\n";
                    destPiecePtr = std::make_unique<Queen>(pieceColor);
                    break;
            }
        }
    }

    //Switching turns after player move
    switchTurn();

    return true;
}

void ChessGameEngine::displayGame() const {
    board.printBoard();
    std::cout << "Current turn: " << (currentTurn == Color::White ? "White" : "Black") << std::endl;
}

bool ChessGameEngine::isMoveLegal(const ChessMoves& move) const {
    //Check if source position has a piece of the current player
    const auto& sourcePiecePtr = board.getPiece(move.initialRow, move.initialCol);
    if (!sourcePiecePtr || sourcePiecePtr->getColor() != currentTurn)
        return false;

    //Checking if destination is on board and not occupied by current player's piece
    if (move.desiredRow < 0 || move.desiredRow >= BOARD_SIZE || move.desiredCol < 0 || move.desiredCol >= BOARD_SIZE)
        return false;

    const auto& destPiecePtr = board.getPiece(move.desiredRow, move.desiredCol);
    if (destPiecePtr && destPiecePtr->getColor() == currentTurn)
        return false;

    //Validation of move by the piece
    if (!sourcePiecePtr->isValidMove(move, board))
        return false;

//Adding other rulessets later just testing for now.

    return true;
}

void ChessGameEngine::switchTurn() {
    currentTurn = (currentTurn == Color::White) ? Color::Black : Color::White;
}
