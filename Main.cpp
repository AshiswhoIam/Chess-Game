#include "ChessGameEngine.h"
#include "ChessMoves.h"
#include <iostream>

int main() {
    ChessGameEngine gameEngine;
    int iniRow, iniCol, destRow, destCol;

    std::cout << "Chess Game!\n";
    std::cout << "Lowercase Black pieces, uppercase are White pieces.\n";
    std::cout << "'r' at [0,0] is a Black Rook, 'N' at [7,1] is a White Knight for the positions of board.\n";
    std::cout << "Enter -1  to exit.\n\n";

    //Game loop
    while (true) {
        //Color storage
        Color currentPlayer = gameEngine.getCurrentTurn();

        // Check for checkmate
        if (gameEngine.isCheckmated(currentPlayer)) {
            std::cout << (currentPlayer == Color::White ? "White" : "Black") << " is in checkmate! "
                      << (currentPlayer == Color::White ? "Black" : "White") << " wins!\n";
            break;
        }

        // Check for stalemate
        if (gameEngine.isStalemated(currentPlayer)) {
            std::cout << "Stalemate! The game is a draw.\n";
            break;
        }
        //Display the board & color current turn
        std::cout << "\nCurrent Game Board:\n" << std::endl;
        gameEngine.displayGame();

        //User input based on row/col array
        std::cout << "\nEnter initial row, initial col, destination row, destination col:\n";
        std::cin >> iniRow;
        if (iniRow == -1) {
            std::cout << "Exiting game...\n";
            break;
        }
        std::cin >> iniCol >> destRow >> destCol;

        //Create the move
        ChessMoves userMove(iniRow, iniCol, destRow, destCol);
        bool moved = gameEngine.makeMove(userMove);

        //Check if move legal or not
        if (!moved) {
            std::cout << "\nIllegal move! Please try again.\n";
        } else {
            std::cout << "\nMove succeeded.\n";
        }
    }

    std::cout << "\nFinal Board State:\n";
    gameEngine.displayGame();

    return 0;
}
