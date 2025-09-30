#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

class TextureManager {
public:
    static sf::Texture& GetText(const std::string &name);
    static void loadText(const std::string& name, const std::string& fileName);
    static void Clear();
private:
    static std::map<std::string, sf::Texture> textures;
};

#endif //TEXTUREMANAGER_H
