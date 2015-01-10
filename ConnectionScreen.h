#ifndef CONNECTIONSCREEN_H
#define CONNECTIONSCREEN_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Screen.h"
#include "Meta.h"

class ConnectionScreen : public Screen {
private:
    std::string display_text, troll_quotes;
    sf::Font font;
    sf::Text text, funny_text;
    sf::TcpSocket *socket;
    std::string ip;
    sf::TcpListener listener;
    sf::Texture texture;
    sf::Sprite sprite;
    std::string username, *other;
    int mode;
    bool connected;
    bool init_text();
    bool init_img();
    void update_text(sf::Text &text, std::string str, sf::Vector2f pos);
    void connect_to_other();
public:
    ConnectionScreen(sf::TcpSocket &socket, int mode, std::string ip,
                     std::string username, std::string &other);
    virtual int run(sf::RenderWindow &window);
};

#endif // CONNECTIONSCREEN_H
