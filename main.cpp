#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "board.h"
//func to get players names froma a welcome window
std::string getPlayerName(sf::RenderWindow& window, sf::Font& font, int width, int height) {
    sf::Text title("WELCOME TO MINESWEEPER!", font, 24); //simple welcome window
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    title.setFillColor(sf::Color::White);

    sf::Text prompt("Enter your name:", font, 20);
    prompt.setFillColor(sf::Color::White);

    sf::Text nameDisplay("", font, 18);
    nameDisplay.setFillColor(sf::Color::Yellow);
//below is a helper to make sure that the text on the screen is centered perfectly
    auto centerText = [](sf::Text& text, float x, float y) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2);
        text.setPosition(x, y);
    };
//the intial positioning for the center text.
    centerText(title, width / 2, height / 2 - 100);
    centerText(prompt, width / 2, height / 2 - 50);

    std::string name;
    bool done = false;
//input loop for the welcome window
    while (window.isOpen() && !done) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();//closes entirely
                return "";
            }
            if (event.type == sf::Event::TextEntered) { //handles text entry
                char c = static_cast<char>(event.text.unicode);
                if (std::isalpha(c) && name.length() < 10)
                    name += c;
                else if (c == '\b' && !name.empty())
                    name.pop_back();
            }//have to press enter to submit ur name
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (!name.empty())
                    done = true;
            }
        }
//makes sure that the name is formatted correct (only first is capitalized rst isnt)
        std::string formatted = name;
        if (!formatted.empty()) {
            formatted[0] = std::toupper(formatted[0]);
            for (size_t i = 1; i < formatted.size(); ++i)
                formatted[i] = std::tolower(formatted[i]);
        }
//displays the curosor as instructed
        formatted += "|";
        nameDisplay.setString(formatted);
        centerText(nameDisplay, width / 2, height / 2);
//below text will display the welcome screen in blue
        window.clear(sf::Color(50, 100, 200));
        window.draw(title);
        window.draw(prompt);
        window.draw(nameDisplay);
        window.display();
    }

    return name; //returns whatever the4 user submits
}

int main() {//loads config according to user settings which can be altered as instructed
    std::ifstream config("files/boards/config.cfg");
    int columns, rows, mines;
    config >> columns >> rows >> mines;
    config.close();

    int width = columns * 32;
    int height = rows * 32 + 100;
//loads the game font i switched it to the basic font provided in the Project 3 zip file
    sf::Font font;
    font.loadFromFile("files/font/font.ttf");
//launches the welcome window
    sf::RenderWindow welcome(sf::VideoMode(width, height), "Welcome");
    std::string playerName = getPlayerName(welcome, font, width, height);
    if (playerName.empty()) return 0;
//launches main game window with the player name returned above
    sf::RenderWindow gameWindow(sf::VideoMode(width, height), "Minesweeper");
    Board board(columns, rows, mines, playerName, font);
    board.run(gameWindow);
    return 0;
}

