#include "msx_player.h"
#include "tinyfiledialogs.h"
#include <iostream>

int main() {
    // Define the fixed view size (same as the original design)
    const float VIEW_WIDTH = 800.0f;
    const float VIEW_HEIGHT = 600.0f;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Music Player");
    window.setFramerateLimit(60);

    // Set up the view to prevent stretching
    sf::View view(sf::FloatRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT));
    window.setView(view);

    // Load font
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

            // Handle window resizing
            if (event.type == sf::Event::Resized) {
                // Update the view to maintain aspect ratio
                float windowWidth = static_cast<float>(event.size.width);
                float windowHeight = static_cast<float>(event.size.height);
                float viewRatio = VIEW_WIDTH / VIEW_HEIGHT;
                float windowRatio = windowWidth / windowHeight;

                sf::FloatRect viewport(0, 0, 1, 1);
                if (windowRatio > viewRatio) {
                    // Window is wider than the view
                    float width = viewRatio / windowRatio;
                    viewport.width = width;
                    viewport.left = (1.0f - width) / 2.0f;
                } else {
                    // Window is taller than the view
                    float height = windowRatio / viewRatio;
                    viewport.height = height;
                    viewport.top = (1.0f - height) / 2.0f;
                }
                view.setViewport(viewport);
                window.setView(view);
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
        sf::RectangleShape menuBar(sf::Vector2f(VIEW_WIDTH, 30));
        menuBar.setFillColor(sf::Color(50, 50, 50));
        menuBar.setPosition(0, 0);
        window.draw(menuBar);

        sf::Text menuText("File", font, 20);
        menuText.setFillColor(sf::Color::White);
        menuText.setPosition(10, 5);
        window.draw(menuText);

        // Draw Control Panel
        sf::RectangleShape controlPanel(sf::Vector2f(VIEW_WIDTH - 60, 100));
        controlPanel.setFillColor(sf::Color(200, 200, 200));
        controlPanel.setPosition(30, VIEW_HEIGHT - 150);
        window.draw(controlPanel);

        // Draw Playlist Area
        sf::RectangleShape playlistArea(sf::Vector2f(VIEW_WIDTH - 60, VIEW_HEIGHT - 200));
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