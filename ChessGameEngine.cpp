#include "ChessGameEngine.h"
#include "ChessBoard.h"
#include <iostream>
#include <cctype>

//Constructor to initialize board with pieces
ChessGameEngine::ChessGameEngine() : currentTurn(Color::White) {

}

//Validation of the moves done
bool ChessGameEngine::makeMove(const ChessMoves& move) {

    auto& sourcePiecePtr = board.getPiece(move.initialRow, move.initialCol);
    //Get the piece at the source/ini position
    //auto& sourcePiecePtr = board.getPiece(move.initialRow, move.initialCol);
    if (!sourcePiecePtr) {
        std::cout << "No piece at the source position." << std::endl;
        return false;
    }
    char symbol = sourcePiecePtr->getSymbol();
    Color pieceColor = sourcePiecePtr->getColor();

    //Check if the move is a castling move
    bool isKingside = (move.desiredCol == 6);
    bool isQueenside = (move.desiredCol == 2);
    bool isCastlingRow = (pieceColor == Color::White) ? (move.initialRow == 7) : (move.initialRow == 0);

    if ((symbol == 'K' || symbol == 'k') &&
        move.initialCol == 4 && isCastlingRow &&
        (isKingside || isQueenside)) {

        if (canCastle(pieceColor, isKingside)) {
            performCastling(pieceColor, isKingside);

            //Update castling flags after castling move
            if (pieceColor == Color::White) {
                whiteKingMoved = true;
                if (isKingside)
                    whiteRookKingsideMoved = true;
                else
                    whiteRookQueensideMoved = true;
            } else {
                blackKingMoved = true;
                if (isKingside)
                    blackRookKingsideMoved = true;
                else
                    blackRookQueensideMoved = true;
            }

            switchTurn();
            return true;
        } else {
            std::cout << "Castling not allowed.\n";
            return false;
        }
        }

    if (!isMoveLegal(move)) {
        std::cout << "Illegal move has been made!" << std::endl;
        return false;
    }





    //Update castling flags if King or Rook moves normally
    if (symbol == 'K') {
        whiteKingMoved = true;
    } else if (symbol == 'k') {
        blackKingMoved = true;
    } else if (symbol == 'R') {
        if (move.initialRow == 7 && move.initialCol == 0)
            whiteRookQueensideMoved = true;
        else if (move.initialRow == 7 && move.initialCol == 7)
            whiteRookKingsideMoved = true;
    } else if (symbol == 'r') {
        if (move.initialRow == 0 && move.initialCol == 0)
            blackRookQueensideMoved = true;
        else if (move.initialRow == 0 && move.initialCol == 7)
            blackRookKingsideMoved = true;
    }

    //Get the piece at the destination
    auto& destPiecePtr = board.getPiece(move.desiredRow, move.desiredCol);

    //Capturing if the dest piece is not of same color
    if (destPiecePtr && destPiecePtr->getColor() != sourcePiecePtr->getColor()) {
        //Remove the dest piece that is captured
        destPiecePtr.reset();
    }

    //Move the piece here transferring unique ptr
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

    //Use the helper method instead of creating a temporary engine
    if (isKingInCheckAfterMove(move, currentTurn)) {
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
    bool foundKing = false;
    for (int row = 0; row < BOARD_SIZE && !foundKing; ++row) {
        for (int col = 0; col < BOARD_SIZE && !foundKing; ++col) {
            const auto& piece = board.getPiece(row, col);
            if (piece && piece->getSymbol() == (kingColor == Color::White ? 'K' : 'k')) {
                kingRow = row;
                kingCol = col;
                foundKing = true;
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

//Checking for legal moves available for king checkmate
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

Color ChessGameEngine::getCurrentTurn() const {
    return currentTurn;
}

//Can castle method
bool ChessGameEngine::canCastle(Color color, bool kingside) const {
    int row = (color == Color::White) ? 7 : 0;
    int kingCol = 4;
    int rookCol = kingside ? 7 : 0;

    //Check if king or relevant rook has moved
    if ((color == Color::White && (whiteKingMoved || (kingside ? whiteRookKingsideMoved : whiteRookQueensideMoved))) ||
        (color == Color::Black && (blackKingMoved || (kingside ? blackRookKingsideMoved : blackRookQueensideMoved)))) {
        return false;
    }

    //Verify king and rook are actually present
    const auto& kingPiece = board.getPiece(row, kingCol);
    const auto& rookPiece = board.getPiece(row, rookCol);

    if (!kingPiece || !rookPiece ||
        kingPiece->getColor() != color || rookPiece->getColor() != color ||
        kingPiece->getSymbol() != (color == Color::White ? 'K' : 'k') ||
        rookPiece->getSymbol() != (color == Color::White ? 'R' : 'r')) {
        return false;
    }

    //Check if squares between king and rook are empty
    int startCol = std::min(kingCol, rookCol) + 1;
    int endCol = std::max(kingCol, rookCol) - 1;

    for (int col = startCol; col <= endCol; ++col) {
        if (board.getPiece(row, col)) return false;
    }

    //Check if king is currently in check
    if (isKingInCheck(color)) {
        return false;
    }

    //Checking king doesn't pass through attacked squares
    //Use existing board.isSquareUnderAttack method instead of creating temp engines
    std::vector<int> pathCols = kingside ? std::vector<int>{5, 6} : std::vector<int>{3, 2};
    for (int col : pathCols) {
        if (board.isSquareUnderAttack(row, col, color)) {
            return false;
        }
    }

    return true;
}

//Helping to check if move king in check
bool ChessGameEngine::isKingInCheckAfterMove(const ChessMoves& move, Color kingColor) const {
    //Creating a copy of the board to simulate the move
    ChessBoard tempBoard = board;

    //Performing the move on the temporary board
    auto& tempSource = tempBoard.getPiece(move.initialRow, move.initialCol);
    auto& tempDest = tempBoard.getPiece(move.desiredRow, move.desiredCol);
    //No piece to move
    if (!tempSource) return false;

    tempDest = std::move(tempSource);
    tempSource = nullptr;

    //Checking if the king is in check on this temporary board
    //Findng king
    int kingRow = -1, kingCol = -1;
    bool foundKing = false;

    for (int row = 0; row < BOARD_SIZE && !foundKing; ++row) {
        for (int col = 0; col < BOARD_SIZE && !foundKing; ++col) {
            const auto& piece = tempBoard.getPiece(row, col);
            if (piece && piece->getSymbol() == (kingColor == Color::White ? 'K' : 'k')) {
                kingRow = row;
                kingCol = col;
                foundKing = true;
            }
        }
    }
    //If no king found, assume check
    if (!foundKing) return true;

    //Check if any enemy piece can attack the king
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            const auto& piece = tempBoard.getPiece(row, col);
            if (piece && piece->getColor() != kingColor) {
                ChessMoves testMove(row, col, kingRow, kingCol);
                //King would be in check
                if (piece->isValidMove(testMove, tempBoard)) {
                    return true;
                }
            }
        }
    }
    //King would not be in check
    return false;
}

//Doing the Castling
void ChessGameEngine::performCastling(Color color, bool kingside) {
    int row = (color == Color::White) ? 7 : 0;
    int kingCol = 4;
    int rookCol = kingside ? 7 : 0;

    int newKingCol = kingside ? 6 : 2;
    int newRookCol = kingside ? 5 : 3;

    //Move King
    auto& kingPtr = board.getPiece(row, kingCol);
    auto& kingDest = board.getPiece(row, newKingCol);
    kingDest = std::move(kingPtr);
    board.getPiece(row, kingCol) = nullptr;

    //Move Rook
    auto& rookPtr = board.getPiece(row, rookCol);
    auto& rookDest = board.getPiece(row, newRookCol);
    rookDest = std::move(rookPtr);
    board.getPiece(row, rookCol) = nullptr;

    //Update castling rights
    if (color == Color::White) {
        whiteKingMoved = true;
        if (kingside)
            whiteRookKingsideMoved = true;
        else
            whiteRookQueensideMoved = true;
    } else {
        blackKingMoved = true;
        if (kingside)
            blackRookKingsideMoved = true;
        else
            blackRookQueensideMoved = true;
    }
}

//Checking if square is under a opposing threat
bool ChessGameEngine::isSquareUnderAttack(int row, int col, Color threatenedColor) const {
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            const auto& piece = board.getPiece(r, c);
            if (piece && piece->getColor() != threatenedColor) {
                ChessMoves move(r, c, row, col);
                //true then this square is under attack
                if (piece->isValidMove(move, board)) {
                    return true;
                }
            }
        }
    }
    return false;
}
//returing the board
const ChessBoard& ChessGameEngine::getBoard() const {
    return board;
}
