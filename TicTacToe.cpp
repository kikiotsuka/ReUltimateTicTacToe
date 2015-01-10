#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "Input.h"
#include "Selector.h"
#include "ConnectionScreen.h"
#include "Game.h"
#include "Meta.h"

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(S_WIDTH, S_HEIGHT), "Ultimate Tic Tac Toe - By Mitsuru Otsuka", sf::Style::Close, settings);

    window.setFramerateLimit(FPS);

    //DEBUG CODE
    //sf::TcpSocket tcp;
    //int num = 0;
    //std::string meh = "hello";
    //Game game(tcp, num, meh, meh);
    //game.run(window);
    //return -1;
    //DEBUG CODE END

    std::string username = "";
    std::string other = "";
    std::string ip = SERVER_IP;
    int status = 0;
    sf::TcpSocket socket;
    int mode = -1;

    std::vector<Screen*> screens;
    screens.push_back(new Input(username, "Enter your name (Max 20 Characters)"));
    screens.push_back(new Selector(mode));

    while (status != -1 && status < screens.size()) {
        status = screens[status]->run(window);
    }
    if (status == -1) return -1;

    screens.push_back(new Input(ip, "Enter server ip"));
    screens.push_back(new ConnectionScreen(socket, mode, ip, username, other));

    while (status != -1 && status < screens.size()) {
        status = screens[status]->run(window);
    }
    if (status == -1) return -1;

    screens.push_back(new Game(socket, mode, username, other));
    status = screens[status]->run(window);

    return 0;
}
