#include "TextureManager.h"

std::map<std::string, sf::Texture> TextureManager::textures;

void TextureManager::loadText(const std::string& name, const std::string& fileName) {
    sf::Texture texture;
    if (texture.loadFromFile(fileName)) {
        textures[name] = texture;
    }
}

sf::Texture& TextureManager::GetText(const std::string& name) {
    return textures.at(name);
}

void TextureManager::Clear() {
    textures.clear();
}
