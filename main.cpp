#include "msx_player.h"
#include "tinyfiledialogs.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <filesystem>

class MusicPlayerUI {
private:
    sf::RenderWindow window;
    sf::View view;
    sf::Font font;
    MusicPlayer player;
    std::vector<std::string> folderPaths;
    
    Button selectFolderButton, playButton, pauseButton, stopButton, nextButton, prevButton, exitButton;
    float scrollOffset = 0.0f;
    int hoveredTrack = -1;
    bool clickProcessed = false;

    static constexpr float PLAYLIST_TOP = 60.0f;
    static constexpr float PLAYLIST_BOTTOM = 500.0f;
    static constexpr float TRACK_HEIGHT = 40.0f;

public:
    MusicPlayerUI() 
        : window(sf::VideoMode(800, 600), "MSX Player", sf::Style::Resize),
          view(sf::FloatRect(0, 0, 800.0f, 600.0f)),
          selectFolderButton("+ Add Folder", 50, 10, 180, 40, sf::Color(0, 255, 255, 200), font),
          playButton("Play", 300, 550, 50, 50, sf::Color(0, 255, 0, 200), font),
          pauseButton("Pause", 360, 550, 50, 50, sf::Color(255, 255, 0, 200), font),
          stopButton("Stop", 420, 550, 50, 50, sf::Color(255, 0, 0, 200), font),
          nextButton("Next", 480, 550, 50, 50, sf::Color(255, 100, 255, 200), font),
          prevButton("Previous", 240, 550, 50, 50, sf::Color(100, 255, 255, 200), font),
          exitButton("Exit", 680, 550, 80, 50, sf::Color(255, 50, 50, 200), font) {
        
        window.setFramerateLimit(60);
        window.setView(view);
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            std::cout << "Warning: Font Load Failed!\n";
        }
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            render();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) {
                view.setSize(800.0f, 600.0f);
                float aspectRatio = 800.0f / 600.0f;
                float windowRatio = static_cast<float>(event.size.width) / event.size.height;
                sf::FloatRect viewport(0, 0, 1, 1);
                if (windowRatio > aspectRatio) {
                    viewport.width = aspectRatio / windowRatio;
                    viewport.left = (1.0f - viewport.width) / 2.0f;
                } else {
                    viewport.height = windowRatio / aspectRatio;
                    viewport.top = (1.0f - viewport.height) / 2.0f;
                }
                view.setViewport(viewport);
                window.setView(view);
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                scrollOffset -= event.mouseWheelScroll.delta * 30;
                clampScrollOffset();
            }
            if (event.type == sf::Event::MouseButtonPressed && !clickProcessed) {
                handleMouseClick(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                clickProcessed = true;
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                clickProcessed = false;
            }
            if (event.type == sf::Event::MouseMoved) {
                handleMouseHover(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
            }
        }
    }

    void handleMouseClick(sf::Vector2f mousePos) {
        // Button actions first (higher priority)
        if (selectFolderButton.contains(mousePos)) {
            std::cout << "Select Folder button Clicked\n";
            const char* folderPath = tinyfd_selectFolderDialog("Select Music Folder", "");
            if (folderPath) {
                folderPaths.push_back(folderPath);
                player.loadFromFolder(folderPath);
                scrollOffset = 0.0f;
            }
            return;  // Prevent track click
        } 
        if (playButton.contains(mousePos)) {
            std::cout << "Play button Clicked\n";
            if (!player.getPlaylist().empty()) {
                player.play();
                adjustScrollToCurrent();
            }
            return;
        } 
        if (pauseButton.contains(mousePos)) {
            std::cout << "Pause button Clicked\n";
            player.pause();
            return;
        } 
        if (stopButton.contains(mousePos)) {
            std::cout << "Stop button Clicked\n";
            player.stop();
            return;
        } 
        if (nextButton.contains(mousePos)) {
            std::cout << "Next button Clicked\n";
            player.next();
            adjustScrollToCurrent();
            return;
        } 
        if (prevButton.contains(mousePos)) {
            std::cout << "Previous button Clicked\n";
            player.previous();
            adjustScrollToCurrent();
            return;
        } 
        if (exitButton.contains(mousePos)) {
            std::cout << "Close button Clicked\n";
            window.close();
            return;
        }

        // Track selection only if no button was clicked
        float yOffset = PLAYLIST_TOP - scrollOffset;
        size_t trackIndex = 0;
        for (const auto& track : player.getPlaylist()) {
            sf::FloatRect trackBounds(50, yOffset, 700, TRACK_HEIGHT);
            if (trackBounds.contains(mousePos)) {
                std::cout << "Track " << trackIndex << " Clicked\n";
                player.setTrack(trackIndex);
                adjustScrollToCurrent();
                break;
            }
            yOffset += TRACK_HEIGHT;
            trackIndex++;
        }
    }

    void handleMouseHover(sf::Vector2f mousePos) {
        float yOffset = PLAYLIST_TOP - scrollOffset;
        hoveredTrack = -1;
        size_t trackIndex = 0;
        for (const auto& track : player.getPlaylist()) {
            sf::FloatRect trackBounds(50, yOffset, 700, TRACK_HEIGHT);
            if (trackBounds.contains(mousePos)) {
                hoveredTrack = trackIndex;
                break;
            }
            yOffset += TRACK_HEIGHT;
            trackIndex++;
        }
    }

    void render() {
        window.clear(sf::Color(10, 10, 20));
        selectFolderButton.draw(window);
        playButton.draw(window);
        pauseButton.draw(window);
        stopButton.draw(window);
        nextButton.draw(window);
        prevButton.draw(window);
        exitButton.draw(window);
        renderPlaylist();
        window.display();
    }

    void renderPlaylist() {
        sf::Text trackList;
        trackList.setFont(font);
        trackList.setCharacterSize(16);
        float yOffset = PLAYLIST_TOP - scrollOffset;
        size_t trackIndex = 0;
        for (const auto& track : player.getPlaylist()) {
            std::string trackName = track.substr(track.find_last_of("/") + 1);
            if (trackName.length() > 50) trackName = trackName.substr(0, 47) + "...";
            trackList.setString(std::to_string(trackIndex + 1) + ". " + trackName);
            float textWidth = trackList.getLocalBounds().width;
            float highlightWidth = std::max(textWidth + 20, 700.0f);

            if (yOffset >= PLAYLIST_TOP - TRACK_HEIGHT && yOffset <= PLAYLIST_BOTTOM) {
                sf::RectangleShape highlightBox(sf::Vector2f(highlightWidth, TRACK_HEIGHT));
                highlightBox.setPosition(50, yOffset);
                if (trackIndex == player.getCurrentTrack() && player.getIsPlaying()) {
                    highlightBox.setFillColor(sf::Color(0, 255, 0, 150));
                    trackList.setFillColor(sf::Color::Black);
                } else if (trackIndex == player.getCurrentTrack() || trackIndex == hoveredTrack) {
                    highlightBox.setFillColor(sf::Color(0, 255, 255, 150));
                    trackList.setFillColor(sf::Color::Black);
                } else {
                    highlightBox.setFillColor(sf::Color(0, 0, 0, 0));
                    trackList.setFillColor(sf::Color(0, 255, 255));
                }
                window.draw(highlightBox);
                trackList.setPosition(50, yOffset);
                window.draw(trackList);
            }
            yOffset += TRACK_HEIGHT;
            trackIndex++;
        }
    }

    void adjustScrollToCurrent() {
        float currentY = PLAYLIST_TOP + player.getCurrentTrack() * TRACK_HEIGHT - scrollOffset;
        if (currentY < PLAYLIST_TOP) {
            scrollOffset -= (PLAYLIST_TOP - currentY);
        } else if (currentY + TRACK_HEIGHT > PLAYLIST_BOTTOM) {
            scrollOffset += (currentY + TRACK_HEIGHT - PLAYLIST_BOTTOM);
        }
        clampScrollOffset();
    }

    void clampScrollOffset() {
        float maxScroll = std::max(0.0f, static_cast<float>(player.getPlaylist().size()) * TRACK_HEIGHT - (PLAYLIST_BOTTOM - PLAYLIST_TOP));
        scrollOffset = std::max(0.0f, std::min(scrollOffset, maxScroll));
    }
};

int main() {
    MusicPlayerUI app;
    app.run();
    return 0;
}