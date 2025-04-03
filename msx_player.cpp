#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>

class MusicPlayer {
private:
    sf::Music music;
    std::vector<std::string> playlist;
    size_t currentTrack;

public:
    MusicPlayer() : currentTrack(0) {}

    // Add a song to the playlist
    void addToPlaylist(const std::string& filepath) {
        playlist.push_back(filepath);
    }

    // Play the current track
    bool play() {
        if (playlist.empty()) {
            std::cout << "Playlist is empty!\n";
            return false;
        }

        if (!music.openFromFile(playlist[currentTrack])) {
            std::cout << "Error loading file: " << playlist[currentTrack] << "\n";
            return false;
        }
        
        music.play();
        std::cout << "Playing: " << playlist[currentTrack] << "\n";
        return true;
    }

    // Pause the current track
    void pause() {
        music.pause();
        std::cout << "Paused\n";
    }

    // Stop the current track
    void stop() {
        music.stop();
        std::cout << "Stopped\n";
    }

    // Go to next track
    void next() {
        if (currentTrack < playlist.size() - 1) {
            music.stop();
            currentTrack++;
            play();
        } else {
            std::cout << "End of playlist\n";
        }
    }

    // Go to previous track
    void previous() {
        if (currentTrack > 0) {
            music.stop();
            currentTrack--;
            play();
        } else {
            std::cout << "At start of playlist\n";
        }
    }

    // Show current playlist
    void showPlaylist() {
        if (playlist.empty()) {
            std::cout << "Playlist is empty\n";
            return;
        }
        
        std::cout << "\nPlaylist:\n";
        for (size_t i = 0; i < playlist.size(); i++) {
            std::cout << (i == currentTrack ? "-> " : "   ") 
                      << i + 1 << ". " << playlist[i] << "\n";
        }
    }
};

int main() {
    MusicPlayer player;
    std::string command;

    std::cout << "Simple Music Player\n";
    std::cout << "Commands: add <filepath>, play, pause, stop, next, prev, list, quit\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "quit") {
            break;
        }
        else if (command.substr(0, 3) == "add") {
            if (command.length() > 4) {
                std::string filepath = command.substr(4);
                player.addToPlaylist(filepath);
                std::cout << "Added: " << filepath << "\n";
            }
        }
        else if (command == "play") player.play();
        else if (command == "pause") player.pause();
        else if (command == "stop") player.stop();
        else if (command == "next") player.next();
        else if (command == "prev") player.previous();
        else if (command == "list") player.showPlaylist();
        else std::cout << "Unknown command\n";
    }

    return 0;
}
