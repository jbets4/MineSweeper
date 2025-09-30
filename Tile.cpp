#include "Tile.h"
#include "TextureManager.h"

Tile::Tile() {}
//draw tile based on its curr state (paused, revealed, etc.) and game flags
void Tile::draw(sf::RenderWindow& window, bool debug, bool paused) {
    sf::Sprite sprite;
    if (paused || revealed) //show either revealed or hidden depending on state
        sprite.setTexture(TextureManager::GetText("tile_revealed"));
    else
        sprite.setTexture(TextureManager::GetText("tile_hidden"));

    sprite.setPosition(x * 32, y * 32);
    window.draw(sprite);
//draws flag if not paused
    if (flagged && !paused) {
        sf::Sprite flag(TextureManager::GetText("flag"));
        flag.setPosition(x * 32, y * 32);
        window.draw(flag);
    }
//if revealed and has mine, draws a mine
    if (revealed && mine) {
        sf::Sprite mineSprite(TextureManager::GetText("mine"));
        mineSprite.setPosition(x * 32, y * 32);
        window.draw(mineSprite);
    }
//draws number if adj mines present
    if (revealed && neighborMines > 0) {
        sf::Sprite number(TextureManager::GetText("number_" + std::to_string(neighborMines)));
        number.setPosition(x * 32, y * 32);
        window.draw(number);
    }
//the debug mode which will reveal mine positions for the user regardless of game state
    if (debug && mine && !revealed && !paused) {
        sf::Sprite mineSprite(TextureManager::GetText("mine"));
        mineSprite.setPosition(x * 32, y * 32);
        window.draw(mineSprite);
    }
}

void Tile::set_pos(int x, int y) {
    this->x = x;
    this->y = y;
}
//calcs neighboring tiles and counts the mines around this tile
void Tile::find_Neighbors(const std::vector<std::vector<Tile>>& grid, int cols, int rows) {
    neighbors.clear();
    neighborMines = 0;
//loops through adj positions in all 8 directions to find neighbors
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                neighbors.push_back((Tile*)&grid[ny][nx]);
                if (grid[ny][nx].hasMine())
                    neighborMines++;
            }
        }
    }
}

void Tile::SetMine(bool hasMine) { mine = hasMine; }
bool Tile::hasMine() const { return mine; }

void Tile::reveal() { revealed = true; }
bool Tile::isRevealed() const { return revealed; }

void Tile::toggleFlag() { flagged = !flagged; }
bool Tile::isFlagged() const { return flagged; }

void Tile::flag() { flagged = true; }

int Tile::adjMines() const { return neighborMines; }
