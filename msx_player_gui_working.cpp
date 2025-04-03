#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include "tinyfiledialogs.h"

namespace fs = std::filesystem;

// MusicPlayer Class
class MusicPlayer {
private:
    sf::Music music;
    std::vector<std::string> playlist;
    size_t currentTrack;
    bool isPlaying;

public:
    MusicPlayer() : currentTrack(0), isPlaying(false) {}

    void addToPlaylist(const std::string& filepath) {
        playlist.push_back(filepath);
    }

    void loadFromFolder(const std::string& folderPath) {
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

    bool play() {
        if (playlist.empty()) return false;
        if (!music.openFromFile(playlist[currentTrack])) return false;
        music.play();
        isPlaying = true;
        return true;
    }

    void pause() {
        music.pause();
        isPlaying = false;
    }

    void stop() {
        music.stop();
        isPlaying = false;
    }

    void next() {
        if (currentTrack < playlist.size() - 1) {
            stop();
            currentTrack++;
            play();
        }
    }

    void previous() {
        if (currentTrack > 0) {
            stop();
            currentTrack--;
            play();
        }
    }

    const std::vector<std::string>& getPlaylist() const { return playlist; }
    size_t getCurrentTrack() const { return currentTrack; }
    bool getIsPlaying() const { return isPlaying; }
};

// Button Class for Reusability
class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    bool isClicked;

public:
    Button(const std::string& label, float x, float y, float width, float height, const sf::Color& color, sf::Font& font) 
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

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    bool contains(sf::Vector2f point) {
        return shape.getGlobalBounds().contains(point);
    }

    void setClicked(bool clicked) { isClicked = clicked; }
    bool getClicked() const { return isClicked; }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Music Player");
    window.setFramerateLimit(60);

    // Load font in main scope
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cout << "Warning: Could not load system font\n";
    }

    MusicPlayer player;
    bool folderSelected = false;

    // Create Buttons
    Button selectFolderButton("Select Folder", 50, 50, 150, 40, sf::Color(200, 200, 200), font);
    Button playButton("Play", 300, 500, 80, 40, sf::Color::Green, font);
    Button pauseButton("Pause", 390, 500, 80, 40, sf::Color::Yellow, font);
    Button stopButton("Stop", 480, 500, 80, 40, sf::Color::Red, font);
    Button nextButton("Next", 570, 500, 80, 40, sf::Color::Blue, font);
    Button prevButton("Prev", 660, 500, 80, 40, sf::Color::Blue, font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (selectFolderButton.contains(mousePos) && !folderSelected) {
                    const char* folderPath = tinyfd_selectFolderDialog("Select Music Folder", "");
                    if (folderPath) {
                        player.loadFromFolder(folderPath);
                        folderSelected = true;
                    }
                }
                else if (playButton.contains(mousePos)) {
                    player.play();
                }
                else if (pauseButton.contains(mousePos)) {
                    player.pause();
                }
                else if (stopButton.contains(mousePos)) {
                    player.stop();
                }
                else if (nextButton.contains(mousePos)) {
                    player.next();
                }
                else if (prevButton.contains(mousePos)) {
                    player.previous();
                }
            }
        }

        window.clear(sf::Color(240, 240, 240)); // Light gray background

        // Draw Menu Bar
        sf::RectangleShape menuBar(sf::Vector2f(800, 30));
        menuBar.setFillColor(sf::Color(50, 50, 50));
        menuBar.setPosition(0, 0);
        window.draw(menuBar);

        sf::Text menuText("File", font, 20);
        menuText.setFillColor(sf::Color::White);
        menuText.setPosition(10, 5);
        window.draw(menuText);

        // Draw Control Panel
        sf::RectangleShape controlPanel(sf::Vector2f(740, 100));
        controlPanel.setFillColor(sf::Color(200, 200, 200));
        controlPanel.setPosition(30, 450);
        window.draw(controlPanel);

        // Draw Playlist Area
        sf::RectangleShape playlistArea(sf::Vector2f(740, 400));
        playlistArea.setFillColor(sf::Color(255, 255, 255));
        playlistArea.setPosition(30, 30);
        window.draw(playlistArea);

        if (!folderSelected) {
            selectFolderButton.draw(window);
        } else {
            const auto& playlist = player.getPlaylist();
            for (size_t i = 0; i < playlist.size() && i < 15; i++) {
                sf::Text text;
                text.setFont(font);
                text.setString(std::to_string(i + 1) + ". " + fs::path(playlist[i]).filename().string());
                text.setCharacterSize(18);
                text.setFillColor(i == player.getCurrentTrack() && player.getIsPlaying() ? sf::Color::Green : sf::Color::Black);
                text.setPosition(40, 40 + i * 25);
                window.draw(text);
            }
        }

        playButton.draw(window);
        pauseButton.draw(window);
        stopButton.draw(window);
        nextButton.draw(window);
        prevButton.draw(window);

        window.display();
    }

    return 0;
}