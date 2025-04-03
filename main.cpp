// Cyberpunk-Themed Music Player UI & Functionality
// Enhancements: Multiple Folder Selection, Better UI, Long Name Handling, Current Track Highlighting, Next/Previous Controls

#include "msx_player.h"
#include "tinyfiledialogs.h"
#include <iostream>
#include <SFML/Graphics.hpp>

int main() {
    const float VIEW_WIDTH = 800.0f;
    const float VIEW_HEIGHT = 600.0f;

    sf::RenderWindow window(sf::VideoMode(VIEW_WIDTH, VIEW_HEIGHT), "Cyberpunk Music Player", sf::Style::Resize);
    window.setFramerateLimit(60);
    sf::View view(sf::FloatRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT));
    window.setView(view);

    // Load Cyberpunk Font (Linux Path)
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cout << "Warning: Font Load Failed!\n";
    }

    MusicPlayer player;
    std::vector<std::string> folderPaths; // Store multiple folder paths

    // Create Cyberpunk Themed Buttons
    Button selectFolderButton("+ Add Folder", 50, 50, 180, 40, sf::Color(0, 255, 255, 200), font);
    Button playButton("▶ Play", 300, 550, 80, 40, sf::Color(0, 255, 0, 200), font);
    Button pauseButton("⏸ Pause", 390, 550, 80, 40, sf::Color(255, 255, 0, 200), font);
    Button stopButton("⏹ Stop", 480, 550, 80, 40, sf::Color(255, 0, 0, 200), font);
    Button nextButton("⏭ Next", 570, 550, 80, 40, sf::Color(255, 100, 255, 200), font);
    Button prevButton("⏮ Prev", 210, 550, 80, 40, sf::Color(100, 255, 255, 200), font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                view.setSize(event.size.width, event.size.height);
                window.setView(view);
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (selectFolderButton.contains(mousePos)) {
                    const char* folderPath = tinyfd_selectFolderDialog("Select Music Folder", "");
                    if (folderPath) {
                        folderPaths.push_back(folderPath);
                        player.loadFromFolder(folderPath);
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

        window.clear(sf::Color(10, 10, 20)); // Dark Cyberpunk Background

        // Draw Buttons with Glowing Effects
        selectFolderButton.draw(window);
        playButton.draw(window);
        pauseButton.draw(window);
        stopButton.draw(window);
        nextButton.draw(window);
        prevButton.draw(window);

        // Display Playlist with Scrolling and Avoid Overlapping Buttons
        sf::Text trackList;
        trackList.setFont(font);
        trackList.setCharacterSize(16);
        trackList.setFillColor(sf::Color(0, 255, 255));

        int yOffset = 100;
        int trackIndex = 0;
        for (const auto& track : player.getPlaylist()) {
            if (trackIndex == player.getCurrentTrack()) {
                trackList.setFillColor(sf::Color(255, 255, 0)); // Highlight current song
            } else {
                trackList.setFillColor(sf::Color(0, 255, 255));
            }
            trackList.setString(track);
            trackList.setPosition(50, yOffset);
            window.draw(trackList);
            yOffset += 30;

            if (yOffset > 500) break; // Prevent overlapping buttons
            trackIndex++;
        }

        window.display();
    }

    return 0;
}
