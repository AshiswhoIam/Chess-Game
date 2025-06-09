#include "ChessGameEngine.h"
#include "ChessMoves.h"
#include <iostream>
#include "ChessBoard.h"
#include "ChessPiece.h"
#include <unordered_map>
#include <SFML/Graphics.hpp>

int main() {
    //Making the gameengine
    ChessGameEngine gameEngine;

    const int BOARD_SIZE = 640;
    const int SQUARE_SIZE = BOARD_SIZE / 8;

    sf::RenderWindow window(sf::VideoMode(BOARD_SIZE, BOARD_SIZE), "Chess Game");

    //Chess board squares
    sf::RectangleShape squares[8][8];
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            squares[row][col].setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            squares[row][col].setPosition(col * SQUARE_SIZE, row * SQUARE_SIZE);

            if ((row + col) % 2 == 0) {
                squares[row][col].setFillColor(sf::Color(240, 217, 181));
            } else {
                squares[row][col].setFillColor(sf::Color(181, 136, 99));
            }
        }
    }

    //Highlight squares for selected piece and valid moves
    sf::RectangleShape highlightSquare;
    highlightSquare.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
    highlightSquare.setFillColor(sf::Color(255, 255, 0, 128)); // Yellow with transparency

    sf::RectangleShape validMoveSquare;
    validMoveSquare.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
    validMoveSquare.setFillColor(sf::Color(0, 255, 0, 128)); // Green with transparency

    //Loading piece textures
    std::unordered_map<char, std::string> pieceToFile = {
        {'P', "white_pawn.png"},
        {'R', "white_rook.png"},
        {'N', "white_knight.png"},
        {'B', "white_bishop.png"},
        {'Q', "white_queen.png"},
        {'K', "white_king.png"},
        {'p', "black_pawn.png"},
        {'r', "black_rook.png"},
        {'n', "black_knight.png"},
        {'b', "black_bishop.png"},
        {'q', "black_queen.png"},
        {'k', "black_king.png"},
    };

    std::unordered_map<char, sf::Texture> pieceTextures;
    for (auto& [symbol, filename] : pieceToFile) {
        sf::Texture texture;
        std::string fullPath = "assets/" + filename;
        if (!texture.loadFromFile(fullPath)) {
            std::cerr << "Failed to load texture: " << fullPath << std::endl;
        }
        pieceTextures[symbol] = std::move(texture);
    }

    //Game state variables
    bool pieceSelected = false;
    int selectedRow = -1, selectedCol = -1;
    std::vector<std::pair<int, int>> validMoves;

    //Function to get valid moves for a piece
    auto getValidMoves = [&](int row, int col) -> std::vector<std::pair<int, int>> {
        std::vector<std::pair<int, int>> moves;
        const ChessBoard& board = gameEngine.getBoard();
        const auto& piece = board.getPiece(row, col);

        if (!piece || piece->getColor() != gameEngine.getCurrentTurn()) {
            return moves;
        }

        //Check all possible destination squares
        for (int toRow = 0; toRow < 8; toRow++) {
            for (int toCol = 0; toCol < 8; toCol++) {
                ChessMoves move(row, col, toRow, toCol);
                //Using the game engine's move validation
                ChessGameEngine tempEngine = gameEngine;
                if (tempEngine.makeMove(move)) {
                    moves.push_back({toRow, toCol});
                }
            }
        }
        return moves;
    };

    //Function to convert mouse position to board coordinates
    auto mouseToBoard = [&](int mouseX, int mouseY) -> std::pair<int, int> {
        int col = mouseX / SQUARE_SIZE;
        int row = mouseY / SQUARE_SIZE;
        return {row, col};
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    auto [clickedRow, clickedCol] = mouseToBoard(mousePos.x, mousePos.y);

                    //Ensure click is within board bounds
                    if (clickedRow >= 0 && clickedRow < 8 && clickedCol >= 0 && clickedCol < 8) {
                        const ChessBoard& board = gameEngine.getBoard();
                        const auto& clickedPiece = board.getPiece(clickedRow, clickedCol);

                        if (!pieceSelected) {
                            //First click - select a piece
                            if (clickedPiece && clickedPiece->getColor() == gameEngine.getCurrentTurn()) {
                                pieceSelected = true;
                                selectedRow = clickedRow;
                                selectedCol = clickedCol;
                                validMoves = getValidMoves(selectedRow, selectedCol);
                                std::cout << "Selected piece at (" << selectedRow << ", " << selectedCol << ")" << std::endl;
                                std::cout << "Valid moves: " << validMoves.size() << std::endl;
                            }
                        } else {
                            //Second click - attempt to move or reselect
                            if (clickedRow == selectedRow && clickedCol == selectedCol) {
                                //Clicked same square - deselect
                                pieceSelected = false;
                                validMoves.clear();
                                std::cout << "Piece deselected" << std::endl;
                            } else if (clickedPiece && clickedPiece->getColor() == gameEngine.getCurrentTurn()) {
                                //Clicked another piece of same color - reselect
                                selectedRow = clickedRow;
                                selectedCol = clickedCol;
                                validMoves = getValidMoves(selectedRow, selectedCol);
                                std::cout << "Reselected piece at (" << selectedRow << ", " << selectedCol << ")" << std::endl;
                            } else {
                                //Attempt to make a move
                                ChessMoves move(selectedRow, selectedCol, clickedRow, clickedCol);

                                if (gameEngine.makeMove(move)) {
                                    std::cout << "Move successful: (" << selectedRow << ", " << selectedCol
                                              << ") to (" << clickedRow << ", " << clickedCol << ")" << std::endl;
                                    pieceSelected = false;
                                    validMoves.clear();
                                } else {
                                    std::cout << "Invalid move attempted" << std::endl;

                                }
                            }
                        }
                    }
                }
            }

            //Right click to deselect
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    pieceSelected = false;
                    validMoves.clear();
                    std::cout << "Piece deselected (right click)" << std::endl;
                }
            }
        }

        //Rendering
        window.clear();

        //Draw board squares
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                window.draw(squares[row][col]);
            }
        }

        //Draw highlights for selected piece
        if (pieceSelected) {
            highlightSquare.setPosition(selectedCol * SQUARE_SIZE, selectedRow * SQUARE_SIZE);
            window.draw(highlightSquare);
        }

        //Draw valid move highlights
        for (const auto& [row, col] : validMoves) {
            validMoveSquare.setPosition(col * SQUARE_SIZE, row * SQUARE_SIZE);
            window.draw(validMoveSquare);
        }

        //Draw pieces
        const ChessBoard& board = gameEngine.getBoard();
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                const std::unique_ptr<ChessPiece>& piecePtr = board.getPiece(row, col);
                if (piecePtr) {
                    char symbol = piecePtr->getSymbol();
                    if (pieceTextures.find(symbol) != pieceTextures.end()) {
                        sf::Sprite sprite(pieceTextures[symbol]);
                        sprite.setPosition(col * SQUARE_SIZE, row * SQUARE_SIZE);

                        float scaleX = float(SQUARE_SIZE) / pieceTextures[symbol].getSize().x;
                        float scaleY = float(SQUARE_SIZE) / pieceTextures[symbol].getSize().y;
                        sprite.setScale(scaleX, scaleY);

                        window.draw(sprite);
                    }
                }
            }
        }

        window.display();
    }

    return 0;
}