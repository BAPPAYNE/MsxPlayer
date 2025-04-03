#include "msx_player.h"
#include <iostream>

MusicPlayer::MusicPlayer() : currentTrack(0), isPlaying(false) {}

void MusicPlayer::addToPlaylist(const std::string& filepath) {
    playlist.push_back(filepath);
}

void MusicPlayer::loadFromFolder(const std::string& folderPath) {
    std::vector<std::string> supportedExtensions = {".mp3", ".wav", ".ogg", ".flac"};
    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            std::string path = entry.path().string();
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (std::find(supportedExtensions.begin(), supportedExtensions.end(), ext) 
                != supportedExtensions.end()) {
                addToPlaylist(path);
            }
        }
        std::cout << "Loaded " << playlist.size() << " audio files\n";
    } catch (const std::exception& e) {
        std::cout << "Error loading folder: " << e.what() << "\n";
    }
}

bool MusicPlayer::play() {
    if (playlist.empty()) return false;
    if (!music.openFromFile(playlist[currentTrack])) return false;
    music.play();
    isPlaying = true;
    return true;
}

void MusicPlayer::pause() {
    if (isPlaying) {
        music.pause();
        isPlaying = false;
    } else {
        music.play();
        isPlaying = true;
    }
}

void MusicPlayer::stop() {
    music.stop();
    isPlaying = false;
}

void MusicPlayer::next() {
    if (currentTrack < playlist.size() - 1) {
        stop();
        currentTrack++;
        play();
    }
}

void MusicPlayer::previous() {
    if (currentTrack > 0) {
        stop();
        currentTrack--;
        play();
    }
}

const std::vector<std::string>& MusicPlayer::getPlaylist() const {
    return playlist;
}

size_t MusicPlayer::getCurrentTrack() const {
    return currentTrack;
}

bool MusicPlayer::getIsPlaying() const {
    return isPlaying;
}

Button::Button(const std::string& label, float x, float y, float width, float height, const sf::Color& color, sf::Font& font) 
    : isClicked(false) {
    shape.setSize(sf::Vector2f(width, height));
    shape.setPosition(x, y);
    shape.setFillColor(color);

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::Black);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(x + width / 2.0f, y + height / 2.0f);
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

bool Button::contains(sf::Vector2f point) {
    return shape.getGlobalBounds().contains(point);
}

void Button::setClicked(bool clicked) {
    isClicked = clicked;
}

bool Button::getClicked() const {
    return isClicked;
}