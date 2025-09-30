#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
//represents a single cell on the board
class Tile {
public:
    Tile(); //default constructor
//renders the tile on the screen
    void draw(sf::RenderWindow& window, bool debug, bool paused);
    void set_pos(int x, int y); //set pos of tile in the grid
    void find_Neighbors(const std::vector<std::vector<Tile>>& grid, int cols, int rows); //calculates how much mines nearby
    void SetMine(bool hasMine); //control mine palcement
    bool hasMine() const; //this one also controls mine placement
    void reveal();//reveal the tile
    bool isRevealed() const;// reveals
    void toggleFlag();//toggle flag state
    bool isFlagged() const;
    void flag(); // force flag for victory
    int adjMines() const; //returns the number of adj mines

private:
    bool mine = false;
    bool revealed = false;
    bool flagged = false;
    int x = 0, y = 0; //pos in grid
    int neighborMines = 0;
    std::vector<Tile*> neighbors;
};
