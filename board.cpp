#include "board.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>


struct LeaderboardEntry {
    std::string name;
    int time;

    bool operator<(const LeaderboardEntry& other) const {
        return time < other.time;
    }
};
//set up board an UI - loads all textures
Board::Board(int cols, int rows, int mines, const std::string& name, sf::Font& font)
    : columns(cols), rows(rows), mineCount(mines), playerName(name), gameFont(font) {

    // Load all textures used in the game
    TextureManager::loadText("tile_hidden", "files/images/tile_hidden.png");
    TextureManager::loadText("tile_revealed", "files/images/tile_revealed.png");
    TextureManager::loadText("flag", "files/images/flag.png");
    TextureManager::loadText("mine", "files/images/mine.png");

    for (int i = 1; i <= 8; ++i) {
        TextureManager::loadText("number_" + std::to_string(i),
                                  "files/images/number_" + std::to_string(i) + ".png");
    }
    //these are ui textures
    digitsTexture.loadFromFile("files/images/digits.png");
    faceHappy.loadFromFile("files/images/face_happy.png");
    faceWin.loadFromFile("files/images/face_win.png");
    faceLose.loadFromFile("files/images/face_lose.png");
    debugTexture.loadFromFile("files/images/debug.png");
    pauseTexture.loadFromFile("files/images/pause.png");
    playTexture.loadFromFile("files/images/play.png");
    leaderboardTexture.loadFromFile("files/images/leaderboard.png");

    faceSprite.setTexture(faceHappy);
    debugSprite.setTexture(debugTexture);
    pauseSprite.setTexture(pauseTexture);
    playSprite.setTexture(playTexture);
    leaderboardSprite.setTexture(leaderboardTexture);

    resetBrd();
}
//the game loop handles logic, input
void Board::run(sf::RenderWindow& window) {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                HandleClick(sf::Mouse::getPosition(window),
                          event.mouseButton.button == sf::Mouse::Left);
            }
        }

        window.clear(sf::Color::White);
        Draw_Tiles(window);
        DrawUI(window);
        window.display();
    }
}
//draws ui elements like smiley face, pause button, etc.
void Board::DrawUI(sf::RenderWindow& window) {
    int y = rows * 32 + 16;

    faceSprite.setPosition(columns * 16 - 32, y);
    debugSprite.setPosition(columns * 32 - 304, y);
    (paused ? playSprite : pauseSprite).setPosition(columns * 32 - 240, y);
    leaderboardSprite.setPosition(columns * 32 - 176, y);
//updates face based on game status
    if (gameWon)
        faceSprite.setTexture(faceWin);
    else if (gameOver)
        faceSprite.setTexture(faceLose);
    else
        faceSprite.setTexture(faceHappy);

    window.draw(faceSprite);
    window.draw(debugSprite);
    window.draw(paused ? playSprite : pauseSprite);
    window.draw(leaderboardSprite);

    drawCounter(window, mineCount - flagsPlaced, 33, y + 16);
    int seconds = (paused || leaderboardOpen) ? static_cast<int>(pausedTime)
                                            : static_cast<int>(clock.getElapsedTime().asSeconds());
    drawCounter(window, seconds, columns * 32 - 97, y + 16);
}

void Board::Draw_Tiles(sf::RenderWindow& window) {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            board[r][c].draw(window, debug, paused || leaderboardOpen);
        }
    }
}
//handles all mouse clicks - seperates left and right click behavior
void Board::HandleClick(sf::Vector2i pos, bool leftClick) {
    // Check UI buttons first
    sf::FloatRect faceBounds(columns * 16 - 32, rows * 32 + 16, 64, 64);
    sf::FloatRect debugBounds(columns * 32 - 304, rows * 32 + 16, 64, 64);
    sf::FloatRect pauseBounds(columns * 32 - 240, rows * 32 + 16, 64, 64);
    sf::FloatRect leaderBounds(columns * 32 - 176, rows * 32 + 16, 64, 64);

    if (faceBounds.contains(pos.x, pos.y)) {
        resetBrd();
        return;
    }
    else if (debugBounds.contains(pos.x, pos.y) && !gameOver && !gameWon && !paused) {
        debug = !debug;
        return;
    }
    else if (pauseBounds.contains(pos.x, pos.y) && !gameOver && !gameWon) {
        paused = !paused;
        if (paused) {
            pausedTime = clock.getElapsedTime().asSeconds();
        } else {
            clock.restart();
        }
        return;
    }
    else if (leaderBounds.contains(pos.x, pos.y)) {
        openLeaderbrd();  // Open a new leaderboard window
        return;
    }

    // Handle tile clicks
    if (paused || gameOver || leaderboardOpen) return;

    int x = pos.x / 32;
    int y = pos.y / 32;
    if (x < 0 || x >= columns || y < 0 || y >= rows) return;

    Tile& tile = board[y][x];

    if (leftClick) {
        if (tile.isFlagged()) return; //flagged tiles arent revealed

        if (tile.hasMine()) {
            gameOver = true;
            for (auto& row : board) {
                for (Tile& t : row) {
                    if (t.hasMine()) t.reveal();
                }
            }
        } else {
            Reveal_tile(x, y);
            if (checkWinCondition()) {
                gameWon = true;
                for (auto& row : board) {
                    for (Tile& t : row) {
                        if (t.hasMine() && !t.isFlagged()) {
                            t.flag();
                            flagsPlaced++;
                        }
                    }
                }
            }
        }
    } else {
        if (!tile.isRevealed()) {
            tile.toggleFlag();
            flagsPlaced += tile.isFlagged() ? 1 : -1;
        }
    }
}

void Board::Reveal_tile(int x, int y) {
    if (x < 0 || x >= columns || y < 0 || y >= rows) return;
    Tile& tile = board[y][x];

    if (tile.isRevealed() || tile.isFlagged()) return;

    tile.reveal();

    if (tile.adjMines() == 0) {
        RecurReveal(x, y);
    }
}
//this function uses a flood reveal froma  zero-mine tile
void Board::RecurReveal(int x, int y) {
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < columns && ny >= 0 && ny < rows) {
                Reveal_tile(nx, ny);
            }
        }
    }
}
//checks win condition: all non-mine tiles must be revealed
bool Board::checkWinCondition() {
    for (const auto& row : board) {
        for (const Tile& tile : row) {
            if (!tile.hasMine() && !tile.isRevealed()) {
                return false;
            }
        }
    }
    return true;
}
//resets boards
void Board::resetBrd() {
    board.clear();
    board.resize(rows, std::vector<Tile>(columns));
    flagsPlaced = 0;
    gameOver = false;
    gameWon = false;
    paused = false;
    debug = false;
    leaderboardOpen = false;
    clock.restart();
//debug
//std::cout<<"[DEBUG] Board reset initiated.\n"
    place_mines();
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            Tile& t = board[y][x];
            t.set_pos(x, y);
            t.find_Neighbors(board, columns, rows);
        }
    }
    //DEBUG //std::cout<< "[DEBUG] ALL TILES REPOS AND NEIGHBORS SET
}
//randomly places mines on the board at game start
void Board::place_mines() {
    std::vector<bool> mineLocations(columns * rows, false);
    std::fill_n(mineLocations.begin(), mineCount, true);
    std::shuffle(mineLocations.begin(), mineLocations.end(), std::mt19937(std::random_device()()));

    for (int i = 0; i < columns * rows; ++i) {
        int x = i % columns;
        int y = i / columns;
        board[y][x].SetMine(mineLocations[i]);
    }
}

void Board::drawCounter(sf::RenderWindow& window, int value, int x, int y) {
    std::string str = std::to_string(std::abs(value));
    while (str.length() < 3) str = "0" + str;

    if (value < 0) {
        sf::Sprite minus(digitsTexture, sf::IntRect(10 * 21, 0, 21, 32));
        minus.setPosition(x, y);
        window.draw(minus);
        x += 21;
    }

    for (char digit : str) {
        int d = digit - '0';
        sf::Sprite sprite(digitsTexture, sf::IntRect(d * 21, 0, 21, 32));
        sprite.setPosition(x, y);
        window.draw(sprite);
        x += 21;
    }
}
//this function should update leaderboard.txt (under development rn)
void Board::updateLeaderbrd() {
    if (!gameWon) return;

    int time = static_cast<int>(clock.getElapsedTime().asSeconds());
    std::vector<LeaderboardEntry> entries;

    // Read existing scores
    std::ifstream infile("files/leaderboard.txt");
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string name;
        int t;
        if (iss >> name >> t) {
            entries.push_back({name, t});
        }
    }
    infile.close();

    // Add new score
    entries.push_back({playerName, time});
    std::sort(entries.begin(), entries.end());

    if (entries.size() > 5) entries.resize(5); // Keep top 5 only

    // Write back
    std::ofstream outfile("files/leaderboard.txt");
    for (const auto& entry : entries) {
        outfile << entry.name << " " << entry.time << "\n";
    }
}

//opens the leaderboard and displays top scores
void Board::openLeaderbrd() {
    std::ifstream file("files/leaderboard.txt");
    std::vector<LeaderboardEntry> entries;
    std::string name;
    int score;

    while (file >> name >> score) {
        entries.push_back({name, score});
    }

    sf::RenderWindow lbWindow(sf::VideoMode(300, 300), "Leaderboard");
    std::vector<sf::Text> texts;

    sf::Text title("Top 5 Times", gameFont, 24);
    title.setFillColor(sf::Color::White);
    title.setPosition(80, 20);
    texts.push_back(title);

    int y = 60;
    for (const auto& entry : entries) {
        sf::Text text(entry.name + " - " + std::to_string(entry.time) + "s", gameFont, 20);
        text.setFillColor(sf::Color::Yellow);
        text.setPosition(60, y);
        y += 40;
        texts.push_back(text);
    }

    while (lbWindow.isOpen()) {
        sf::Event e;
        while (lbWindow.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                lbWindow.close();
            }
        }

        lbWindow.clear(sf::Color(30, 30, 30));
        for (auto& text : texts) {
            lbWindow.draw(text);
        }
        lbWindow.display();
    }
}
