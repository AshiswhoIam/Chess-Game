#include "ChessGameEngine.h"
#include "ChessMoves.h"
#include <iostream>
#include "ChessBoard.h"
#include "ChessPiece.h"
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <random>
#include <cmath>

//Cloud particle for capture effects
struct CloudParticle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float life;
    float maxLife;
    float size;
    float expansion;

    CloudParticle(sf::Vector2f pos, sf::Vector2f vel, sf::Color col, float lifetime, float particleSize)
        : position(pos), velocity(vel), color(col), life(lifetime), maxLife(lifetime), size(particleSize), expansion(1.0f) {}

    void update(float deltaTime) {
        position += velocity * deltaTime;
        life -= deltaTime;

        //Fade out and expand over time
        float progress = 1.0f - (life / maxLife);
        float alpha = (life / maxLife) * 100.0f; // Softer fade
        color.a = static_cast<sf::Uint8>(std::max(0.0f, alpha));

        //Expand the cloud particle
        expansion = 1.0f + progress * 2.0f;

        //Slow down velocity (dissipation)
        velocity *= 0.98f;
    }

    bool isAlive() const {
        return life > 0.0f;
    }
};

//Background star for nebula effect
struct BackgroundStar {
    sf::Vector2f position;
    float brightness;
    float twinkleSpeed;
    float phase;
    sf::Color baseColor;

    BackgroundStar(sf::Vector2f pos, sf::Color color)
        : position(pos), brightness(0.5f), baseColor(color) {
        twinkleSpeed = 0.5f + static_cast<float>(rand()) / RAND_MAX * 2.0f;
        phase = static_cast<float>(rand()) / RAND_MAX * 6.28f; // Random phase
    }

    void update(float deltaTime) {
        phase += twinkleSpeed * deltaTime;
        brightness = 0.3f + 0.4f * (0.5f + 0.5f * sin(phase));
    }

    sf::Color getCurrentColor() const {
        sf::Color result = baseColor;
        result.a = static_cast<sf::Uint8>(brightness * 255.0f);
        return result;
    }
};

//Animation structure to track moving pieces
struct PieceAnimation {
    char pieceSymbol;
    sf::Vector2f startPos;
    sf::Vector2f endPos;
    sf::Vector2f currentPos;
    float animationTime;
    float duration;
    bool isActive;

    PieceAnimation() : animationTime(0.0f), duration(0.3f), isActive(false) {}

    void start(char symbol, sf::Vector2f start, sf::Vector2f end, float animDuration = 0.3f) {
        pieceSymbol = symbol;
        startPos = start;
        endPos = end;
        currentPos = start;
        animationTime = 0.0f;
        duration = animDuration;
        isActive = true;
    }

    void update(float deltaTime) {
        if (!isActive) return;

        animationTime += deltaTime;
        float progress = animationTime / duration;

        if (progress >= 1.0f) {
            progress = 1.0f;
            isActive = false;
        }

        //Smooth easing function
        float easeProgress = 1.0f - (1.0f - progress) * (1.0f - progress);

        currentPos.x = startPos.x + (endPos.x - startPos.x) * easeProgress;
        currentPos.y = startPos.y + (endPos.y - startPos.y) * easeProgress;
    }

    bool isFinished() const {
        return !isActive;
    }
};

int main() {
    //Making the gameengine
    ChessGameEngine gameEngine;

    const int BOARD_SIZE = 640;
    const int SQUARE_SIZE = BOARD_SIZE / 8;

    sf::RenderWindow window(sf::VideoMode(BOARD_SIZE, BOARD_SIZE), "Chess");

    //Clock for animation timing
    sf::Clock clock;

    //Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    //Cloud particle system
    std::vector<CloudParticle> cloudParticles;

    //Background stars for nebula effect
    std::vector<BackgroundStar> stars;
    for (int i = 0; i < 150; i++) {
        sf::Vector2f pos(dis(gen) * BOARD_SIZE, dis(gen) * BOARD_SIZE);
        sf::Color colors[] = {
            sf::Color(255, 182, 193), // Light pink
            sf::Color(173, 216, 230), // Light blue
            sf::Color(221, 160, 221), // Plum
            sf::Color(176, 196, 222), // Light steel blue
            sf::Color(255, 218, 185)  // Peach
        };
        sf::Color starColor = colors[rand() % 5];
        stars.emplace_back(pos, starColor);
    }

    //Chess board squares with nebula theme
    sf::RectangleShape squares[8][8];

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            squares[row][col].setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            squares[row][col].setPosition(col * SQUARE_SIZE, row * SQUARE_SIZE);

            if ((row + col) % 2 == 0) {
                // Light squares - cosmic blue with transparency
                squares[row][col].setFillColor(sf::Color(100, 149, 237, 180));
            } else {
                // Dark squares - deep space purple with transparency
                squares[row][col].setFillColor(sf::Color(75, 0, 130, 180));
            }
        }
    }

    //Highlight squares with cosmic glow
    sf::RectangleShape highlightSquare;
    highlightSquare.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
    highlightSquare.setFillColor(sf::Color(255, 20, 147, 220));

    sf::RectangleShape validMoveSquare;
    validMoveSquare.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
    validMoveSquare.setFillColor(sf::Color(0, 255, 127, 120));

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

    //Animation state
    PieceAnimation currentAnimation;
    bool isAnimating = false;
    int animatingFromRow = -1, animatingFromCol = -1;
    int animatingToRow = -1, animatingToCol = -1;

    //Promotion UI elements with space theme
    sf::RectangleShape promotionBackground;
    promotionBackground.setSize(sf::Vector2f(BOARD_SIZE, BOARD_SIZE));
    promotionBackground.setFillColor(sf::Color(25, 25, 112, 200)); // Deep space blue

    sf::RectangleShape promotionPanel;
    promotionPanel.setSize(sf::Vector2f(SQUARE_SIZE * 4, SQUARE_SIZE));
    promotionPanel.setPosition((BOARD_SIZE - SQUARE_SIZE * 4) / 2, (BOARD_SIZE - SQUARE_SIZE) / 2);
    promotionPanel.setFillColor(sf::Color(75, 0, 130, 230)); // Cosmic purple
    promotionPanel.setOutlineThickness(3);
    promotionPanel.setOutlineColor(sf::Color(255, 215, 0)); // Golden border

    //Promotion piece buttons
    std::vector<sf::RectangleShape> promotionButtons(4);
    std::vector<char> promotionPieces = {'Q', 'R', 'B', 'N'};

    for (int i = 0; i < 4; i++) {
        promotionButtons[i].setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        promotionButtons[i].setPosition((BOARD_SIZE - SQUARE_SIZE * 4) / 2 + i * SQUARE_SIZE,
                                       (BOARD_SIZE - SQUARE_SIZE) / 2);
        promotionButtons[i].setFillColor(sf::Color(100, 149, 237, 200));
        promotionButtons[i].setOutlineThickness(2);
        promotionButtons[i].setOutlineColor(sf::Color(255, 215, 0));
    }

    //Font for text
    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Could not load font. Promotion will work without text labels." << std::endl;
    }

    sf::Text promotionText;
    promotionText.setFont(font);
    promotionText.setString("Choose promotion piece:");
    promotionText.setCharacterSize(24);
    promotionText.setFillColor(sf::Color(255, 215, 0)); // Golden text
    promotionText.setPosition((BOARD_SIZE - 300) / 2, (BOARD_SIZE - SQUARE_SIZE) / 2 - 40);

    //Function to create capture cloud effect
    auto createCaptureCloud = [&](int row, int col) {
        sf::Vector2f center(col * SQUARE_SIZE + SQUARE_SIZE/2, row * SQUARE_SIZE + SQUARE_SIZE/2);

        //Create 20-30 cloud particles for a puff effect
        int numParticles = 20 + rand() % 11;
        for (int i = 0; i < numParticles; i++) {
            float angle = dis(gen) * 6.28f; // Random angle
            float speed = 20.0f + dis(gen) * 40.0f; // Slower than sparkles
            sf::Vector2f velocity(cos(angle) * speed, sin(angle) * speed);

            //Cloud colors - greys, whites, and soft colors
            sf::Color colors[] = {
                sf::Color(220, 220, 220), // Light grey
                sf::Color(255, 255, 255), // White
                sf::Color(200, 200, 200), // Medium grey
                sf::Color(240, 240, 240), // Off-white
                sf::Color(190, 190, 220), // Light purple-grey
                sf::Color(220, 220, 190)  // Light yellow-grey
            };
            sf::Color cloudColor = colors[rand() % 6];

            float lifetime = 2.0f + dis(gen) * 1.0f; // Longer lasting than sparkles
            float size = 8.0f + dis(gen) * 12.0f;    // Larger particles

            cloudParticles.emplace_back(center, velocity, cloudColor, lifetime, size);
        }
    };

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

                //Quick basic validation first
                if (toRow == row && toCol == col) continue;

                const auto& destPiece = board.getPiece(toRow, toCol);
                if (destPiece && destPiece->getColor() == piece->getColor()) continue;

                if (!piece->isValidMove(move, board)) continue;

                //Checking if move leaves king in check using temp engine
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

    //Function to start piece animation
    auto startPieceAnimation = [&](int fromRow, int fromCol, int toRow, int toCol, char pieceSymbol) {
        sf::Vector2f startPos(fromCol * SQUARE_SIZE, fromRow * SQUARE_SIZE);
        sf::Vector2f endPos(toCol * SQUARE_SIZE, toRow * SQUARE_SIZE);

        currentAnimation.start(pieceSymbol, startPos, endPos, 0.3f);
        isAnimating = true;
        animatingFromRow = fromRow;
        animatingFromCol = fromCol;
        animatingToRow = toRow;
        animatingToCol = toCol;
    };

    while (window.isOpen()) {
        //Get delta time for smooth animation
        float deltaTime = clock.restart().asSeconds();

        //Update animation
        if (isAnimating) {
            currentAnimation.update(deltaTime);
            if (currentAnimation.isFinished()) {
                isAnimating = false;
            }
        }

        //Update cloud particles
        for (auto it = cloudParticles.begin(); it != cloudParticles.end();) {
            it->update(deltaTime);
            if (!it->isAlive()) {
                it = cloudParticles.erase(it);
            } else {
                ++it;
            }
        }

        //Update background stars
        for (auto& star : stars) {
            star.update(deltaTime);
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    //Check if promotion is pending
                    if (gameEngine.isPromotionPending()) {
                        //Check if click is on promotion buttons
                        for (int i = 0; i < 4; i++) {
                            if (promotionButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                gameEngine.executePromotion(promotionPieces[i]);
                                std::cout << "Promoted to " << promotionPieces[i] << std::endl;
                                break;
                            }
                        }
                    } else if (!isAnimating) { // Only allow moves when not animating
                        //Normal game logic
                        auto [clickedRow, clickedCol] = mouseToBoard(mousePos.x, mousePos.y);

                        if (clickedRow >= 0 && clickedRow < 8 && clickedCol >= 0 && clickedCol < 8) {
                            const ChessBoard& board = gameEngine.getBoard();
                            const auto& clickedPiece = board.getPiece(clickedRow, clickedCol);

                            if (!pieceSelected) {
                                if (clickedPiece && clickedPiece->getColor() == gameEngine.getCurrentTurn()) {
                                    pieceSelected = true;
                                    selectedRow = clickedRow;
                                    selectedCol = clickedCol;
                                    validMoves = getValidMoves(selectedRow, selectedCol);
                                    std::cout << "Selected piece at (" << selectedRow << ", " << selectedCol << ")" << std::endl;
                                }
                            } else {
                                if (clickedRow == selectedRow && clickedCol == selectedCol) {
                                    pieceSelected = false;
                                    validMoves.clear();
                                    std::cout << "Piece deselected" << std::endl;
                                } else if (clickedPiece && clickedPiece->getColor() == gameEngine.getCurrentTurn()) {
                                    selectedRow = clickedRow;
                                    selectedCol = clickedCol;
                                    validMoves = getValidMoves(selectedRow, selectedCol);
                                    std::cout << "Reselected piece at (" << selectedRow << ", " << selectedCol << ")" << std::endl;
                                } else {
                                    ChessMoves move(selectedRow, selectedCol, clickedRow, clickedCol);

                                    //Store the piece symbol before making the move
                                    const auto& movingPiece = board.getPiece(selectedRow, selectedCol);
                                    char pieceSymbol = movingPiece ? movingPiece->getSymbol() : ' ';

                                    //Check if there's a piece being captured
                                    bool isCapture = board.getPiece(clickedRow, clickedCol) != nullptr;

                                    if (gameEngine.makeMove(move)) {
                                        std::cout << "Move successful: (" << selectedRow << ", " << selectedCol
                                                  << ") to (" << clickedRow << ", " << clickedCol << ")" << std::endl;

                                        //Create cloud effect if it was a capture
                                        if (isCapture) {
                                            createCaptureCloud(clickedRow, clickedCol);
                                        }

                                        //Start animation
                                        startPieceAnimation(selectedRow, selectedCol, clickedRow, clickedCol, pieceSymbol);

                                        //Don't deselect if promotion is pending
                                        if (!gameEngine.isPromotionPending()) {
                                            pieceSelected = false;
                                            validMoves.clear();
                                        }
                                    } else {
                                        std::cout << "Invalid move attempted" << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            //Right click to deselect only if no promotion pending and not animating
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right && !gameEngine.isPromotionPending() && !isAnimating) {
                    pieceSelected = false;
                    validMoves.clear();
                    std::cout << "Piece deselected (right click)" << std::endl;
                }
            }
        }

        //Rendering
        window.clear(sf::Color(10, 10, 40)); // Deep space background

        //Draw twinkling stars
        for (const auto& star : stars) {
            sf::CircleShape starShape(1.5f + dis(gen) * 1.0f);
            starShape.setPosition(star.position.x - starShape.getRadius(),
                                star.position.y - starShape.getRadius());
            starShape.setFillColor(star.getCurrentColor());
            window.draw(starShape);
        }

        //Draw board squares
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                window.draw(squares[row][col]);
            }
        }

        //Draw highlights with cosmic glow
        if (pieceSelected && !(isAnimating && selectedRow == animatingFromRow && selectedCol == animatingFromCol)) {
            highlightSquare.setPosition(selectedCol * SQUARE_SIZE, selectedRow * SQUARE_SIZE);
            window.draw(highlightSquare);
        }

        //Draw valid move highlights with pulsing effect
        if (!isAnimating) {
            static float pulseTime = 0;
            pulseTime += deltaTime;
            float pulseBrightness = 0.7f + 0.3f * sin(pulseTime * 4.0f);

            for (const auto& [row, col] : validMoves) {
                validMoveSquare.setPosition(col * SQUARE_SIZE, row * SQUARE_SIZE);
                sf::Color pulseColor = validMoveSquare.getFillColor();
                pulseColor.a = static_cast<sf::Uint8>(pulseBrightness * 120);
                validMoveSquare.setFillColor(pulseColor);
                window.draw(validMoveSquare);
            }
        }

        //Draw pieces (skip the piece being animated at its source/destination)
        const ChessBoard& board = gameEngine.getBoard();
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                // Skip drawing the piece that's currently being animated
                if (isAnimating &&
                    ((row == animatingFromRow && col == animatingFromCol) ||
                     (row == animatingToRow && col == animatingToCol))) {
                    continue;
                }

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

        //Draw the animated piece with cosmic glow
        if (isAnimating && pieceTextures.find(currentAnimation.pieceSymbol) != pieceTextures.end()) {
            sf::Sprite animatedSprite(pieceTextures[currentAnimation.pieceSymbol]);
            animatedSprite.setPosition(currentAnimation.currentPos);

            float scaleX = float(SQUARE_SIZE) / pieceTextures[currentAnimation.pieceSymbol].getSize().x;
            float scaleY = float(SQUARE_SIZE) / pieceTextures[currentAnimation.pieceSymbol].getSize().y;
            animatedSprite.setScale(scaleX, scaleY);

            //Add cosmic glow effect during animation
            animatedSprite.move(0, -8); // Lift the piece more during movement

            //Create a subtle glow around the moving piece
            sf::CircleShape glow(SQUARE_SIZE * 0.6f);
            glow.setPosition(currentAnimation.currentPos.x - SQUARE_SIZE * 0.1f,
                           currentAnimation.currentPos.y - SQUARE_SIZE * 0.1f - 8);
            glow.setFillColor(sf::Color(255, 215, 0, 30));
            window.draw(glow);

            window.draw(animatedSprite);
        }

        //Draw cloud particles
        for (const auto& particle : cloudParticles) {
            sf::CircleShape cloud(particle.size * particle.expansion);
            cloud.setPosition(particle.position.x - particle.size * particle.expansion,
                            particle.position.y - particle.size * particle.expansion);
            cloud.setFillColor(particle.color);
            window.draw(cloud);
        }

        //Draw promotion UI with space theme
        if (gameEngine.isPromotionPending()) {
            window.draw(promotionBackground);
            window.draw(promotionPanel);

            if (font.getInfo().family != "") {
                window.draw(promotionText);
            }

            //Draw promotion buttons with cosmic styling
            Color currentColor = gameEngine.getCurrentTurn();

            for (int i = 0; i < 4; i++) {
                window.draw(promotionButtons[i]);

                char pieceSymbol = (currentColor == Color::White) ? promotionPieces[i] : std::tolower(promotionPieces[i]);

                if (pieceTextures.find(pieceSymbol) != pieceTextures.end()) {
                    sf::Sprite pieceSprite(pieceTextures[pieceSymbol]);
                    pieceSprite.setPosition(promotionButtons[i].getPosition());

                    float scaleX = float(SQUARE_SIZE) / pieceTextures[pieceSymbol].getSize().x;
                    float scaleY = float(SQUARE_SIZE) / pieceTextures[pieceSymbol].getSize().y;
                    pieceSprite.setScale(scaleX, scaleY);

                    window.draw(pieceSprite);
                }
            }
        }

        window.display();
    }

    return 0;
}