#include "ChessGameEngine.h"
#include "ChessBoard.h"
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
    //Msg to show check
    Color opponent = (currentTurn == Color::White) ? Color::Black : Color::White;
    if (isKingInCheck(opponent)) {
        std::cout << (opponent == Color::White ? "White" : "Black") << "'s King is in CHECK!" << std::endl;
    }


    //Switching turns after player move
    switchTurn();
    //If its checkmate
    if (!hasLegalMoves(currentTurn)) {
        if (isKingInCheck(currentTurn)) {
            std::cout << (currentTurn == Color::White ? "White" : "Black") << " is in CHECKMATE!" << std::endl;
        } else {
            std::cout << "STALEMATE! No legal moves." << std::endl;
        }
    }

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


    //Make a deep copy of the board to simulate the move
    ChessBoard tempBoard = board;

    //Move pieces on the temp board
    auto& tempSource = tempBoard.getPiece(move.initialRow, move.initialCol);
    auto& tempDest = tempBoard.getPiece(move.desiredRow, move.desiredCol);

    tempDest = std::move(tempSource);
    tempSource = nullptr;

    //Create a temporary engine to check if the current player's king is in check after the move
    ChessGameEngine tempEngine = *this;
    tempEngine.board = std::move(tempBoard);

    //If king is in check after move, it is illegal
    if (tempEngine.isKingInCheck(currentTurn)) {
        return false;
    }


    return true;
}



void ChessGameEngine::switchTurn() {
    currentTurn = (currentTurn == Color::White) ? Color::Black : Color::White;
}

bool ChessGameEngine::isKingInCheck(Color kingColor) const {
    int kingRow = -1, kingCol = -1;

    //Check the kings location
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            const auto& piece = board.getPiece(row, col);
            if (piece && piece->getSymbol() == (kingColor == Color::White ? 'K' : 'k')) {
                kingRow = row;
                kingCol = col;
                break;
            }
        }
    }

    if (kingRow == -1 || kingCol == -1) {
        std::cerr << "Error: King not found on board!\n";
        return false;
    }

    //Checking the opposings moves
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            const auto& piece = board.getPiece(row, col);
            if (piece && piece->getColor() != kingColor) {
                ChessMoves testMove(row, col, kingRow, kingCol);
                //King is checked
                if (piece->isValidMove(testMove, board)) {
                    return true;
                }
            }
        }
    }
    //if King is not checked
    return false;
}

//Checking for legal moves avaiable for king checkmate
bool ChessGameEngine::hasLegalMoves(Color playerColor) const {
    for (int fromRow = 0; fromRow < BOARD_SIZE; ++fromRow) {
        for (int fromCol = 0; fromCol < BOARD_SIZE; ++fromCol) {
            const auto& piece = board.getPiece(fromRow, fromCol);
            if (piece && piece->getColor() == playerColor) {
                for (int toRow = 0; toRow < BOARD_SIZE; ++toRow) {
                    for (int toCol = 0; toCol < BOARD_SIZE; ++toCol) {
                        ChessMoves move(fromRow, fromCol, toRow, toCol);
                        if (isMoveLegal(move)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

//If checkmated
bool ChessGameEngine::isCheckmated(Color playerColor) const {
    return isKingInCheck(playerColor) && !hasLegalMoves(playerColor);
}

//If stalemated
bool ChessGameEngine::isStalemated(Color playerColor) const {
    return !isKingInCheck(playerColor) && !hasLegalMoves(playerColor);
}