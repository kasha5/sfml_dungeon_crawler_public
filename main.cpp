#include "includer.hpp"

int main()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    Game game(desktop.width, desktop.height);

    game.startGame();

    return 0;
}
