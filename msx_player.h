#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

class MusicPlayer {
private:
    sf::Music music;
    std::vector<std::string> playlist;
    size_t currentTrack;
    bool isPlaying;

public:
    MusicPlayer();
    void addToPlaylist(const std::string& filepath);
    void loadFromFolder(const std::string& folderPath);
    bool play();
    void pause();
    void stop();
    void next();
    void previous();
    void setTrack(size_t trackIndex);
    const std::vector<std::string>& getPlaylist() const;
    size_t getCurrentTrack() const;
    bool getIsPlaying() const;
};

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    bool isClicked;

public:
    Button(const std::string& label, float x, float y, float width, float height, const sf::Color& color, sf::Font& font);
    void draw(sf::RenderWindow& window);
    bool contains(sf::Vector2f point);
    void setClicked(bool clicked);
    bool getClicked() const;
};

#endif // MUSIC_PLAYER_H
