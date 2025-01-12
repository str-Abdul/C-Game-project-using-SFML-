#include <SFML/Graphics.hpp>
#include <windows.h> // for SetWindowPos

void ResizeAndCenterRenderWindow(sf::RenderWindow& window, int width, int height) {
    // Resize the SFML window
    window.setSize(sf::Vector2u(width, height));
    
    // Get the screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate the new position to center the window
    int posX = (screenWidth - width) / 2;
    int posY = (screenHeight - height) / 2;

    // Use Windows API to set the position of the SFML window
    HWND hwnd = window.getSystemHandle();
    SetWindowPos(hwnd, HWND_TOP, posX, posY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

int main() {
    // Create an SFML RenderWindow
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Simple Game");
    window.setFramerateLimit(60);

    // Create a player as a square
    sf::RectangleShape player(sf::Vector2f(50.0f, 50.0f));
    player.setFillColor(sf::Color::Green);
    player.setPosition(375.0f, 275.0f); // Center the player in the middle of the window

    // Define player speed
    float playerSpeed = 5.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Resize and center the SFML window when pressing the H key
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
            ResizeAndCenterRenderWindow(window, 800, 600);
        }

        // Move the player based on arrow keys
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            player.move(-playerSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            player.move(playerSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            player.move(0, -playerSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            player.move(0, playerSpeed);
        }

        // Clear, draw, and display
        window.clear(sf::Color::Black);
        window.draw(player);
        window.display();
    }

    return 0;
}


