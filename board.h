#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Tile.h"
#include "TextureManager.h"
//main class for game logic, manages MineSweeper board state, input, and UI
class Board {
public:
    Board(int cols, int rows, int mines, const std::string& playerName, sf::Font& font);
    void run(sf::RenderWindow& window);

private:
    int columns;
    int rows;
    int mineCount;
    std::string playerName;
    sf::Font& gameFont;
    sf::Clock clock;

    // Game state variables
    bool paused = false;
    bool debug = false;
    bool gameOver = false;
    bool gameWon = false;
    bool leaderboardOpen = false;
    int flagsPlaced = 0;
    float pausedTime = 0;

    std::vector<std::vector<Tile>> board;

    // UI elements
    sf::Sprite faceSprite;
    sf::Sprite debugSprite;
    sf::Sprite pauseSprite;
    sf::Sprite playSprite;
    sf::Sprite leaderboardSprite;

    sf::Texture faceHappy, faceWin, faceLose;
    sf::Texture debugTexture, pauseTexture, playTexture, leaderboardTexture;
    sf::Texture digitsTexture;
//internal helper functions (described in cpp better)
    void DrawUI(sf::RenderWindow& window);
    void Draw_Tiles(sf::RenderWindow& window);
    void HandleClick(sf::Vector2i pos, bool leftClick);
    void Reveal_tile(int x, int y);
    void RecurReveal(int x, int y);
    void place_mines();
    bool checkWinCondition();
    void updateGameState();
    void resetBrd();
    void openLeaderbrd();
    void updateLeaderbrd();
    void drawCounter(sf::RenderWindow& window, int value, int x, int y);
};