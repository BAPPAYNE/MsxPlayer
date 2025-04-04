#include "msx_player.h"
#include <iostream>
#include <algorithm>

MusicPlayer::MusicPlayer() : currentTrack(0), isPlaying(false) {}

void MusicPlayer::addToPlaylist(const std::string& filepath) {
    if (std::find(playlist.begin(), playlist.end(), filepath) == playlist.end()) {
        playlist.push_back(filepath);
    }
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
        currentTrack = 0;
        std::cout << "Loaded " << playlist.size() << " unique audio files from " << folderPath << "\n";
        play();
    } catch (const std::exception& e) {
        std::cout << "Error loading folder: " << e.what() << "\n";
    }
}

bool MusicPlayer::play() {
    if (playlist.empty() || currentTrack >= playlist.size()) {
        std::cout << "Cannot play: Invalid track " << currentTrack << "\n";
        return false;
    }
    if (music.getStatus() == sf::Music::Stopped) {
        if (!music.openFromFile(playlist[currentTrack])) {
            std::cout << "Failed to open file: " << playlist[currentTrack] << "\n";
            return false;
        }
        music.play();
        isPlaying = true;
    } else if (music.getStatus() == sf::Music::Paused) {
        music.play();
        isPlaying = true;
    }
    std::cout << "Playing track " << currentTrack << ": " << playlist[currentTrack] << "\n";
    return true;
}

void MusicPlayer::pause() {
    if (isPlaying && music.getStatus() == sf::Music::Playing) {
        music.pause();
        isPlaying = false;
        std::cout << "Paused track " << currentTrack << "\n";
    } else if (!isPlaying && music.getStatus() == sf::Music::Paused) {
        music.play();
        isPlaying = true;
        std::cout << "Resumed track " << currentTrack << "\n";
    }
}

void MusicPlayer::stop() {
    music.stop();
    isPlaying = false;
    std::cout << "Stopped playback\n";
}

void MusicPlayer::next() {
    if (currentTrack + 1 < playlist.size()) {
        stop();
        currentTrack++;
        play();
    } else {
        std::cout << "No next track available\n";
    }
}

void MusicPlayer::previous() {
    if (currentTrack > 0) {
        stop();
        currentTrack--;
        play();
    } else {
        std::cout << "No previous track available\n";
    }
}

void MusicPlayer::setTrack(size_t trackIndex) {
    if (trackIndex < playlist.size()) {
        stop();
        currentTrack = trackIndex;
        play();
    }
}

const std::vector<std::string>& MusicPlayer::getPlaylist() const { return playlist; }
size_t MusicPlayer::getCurrentTrack() const { return currentTrack; }
bool MusicPlayer::getIsPlaying() const { return isPlaying; }

// Button class unchanged

// Button implementation (unchanged)
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
    text.setPosition(x + width / 3.0f, y + height / 2.0f);
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

bool Button::contains(sf::Vector2f point) {
    return shape.getGlobalBounds().contains(point);
}

void Button::setClicked(bool clicked) { isClicked = clicked; }
bool Button::getClicked() const { return isClicked; }