#include "Selector.h"
#include <iostream>
Selector::Selector(int &mode) {
    this->mode = &mode;
}

int Selector::run(sf::RenderWindow &window) {
    if (!init_text()) {
        return -1;
    }
    init_rect();
    update_screen(window);
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            switch(e.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (e.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                break;
            case sf::Event::MouseButtonPressed:
                if (e.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f coord = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
                    if (server_rect.getGlobalBounds().contains(coord)) {
                        *mode = MODE_SERVER;
                        return SCREEN_CONNECTION_WAITING;
                    } else if (client_rect.getGlobalBounds().contains(coord)) {
                        *mode = MODE_CLIENT;
                        return SCREEN_IP_INPUT;
                    }
                }
                break;
            case sf::Event::MouseMoved:
                sf::Vector2f coord = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
                server_rect.setFillColor(sf::Color::White);
                client_rect.setFillColor(sf::Color::White);
                if (server_rect.getGlobalBounds().contains(coord)) {
                    server_rect.setFillColor(sf::Color::Yellow);
                } else if (client_rect.getGlobalBounds().contains(coord)) {
                    client_rect.setFillColor(sf::Color::Yellow);
                }
                update_screen(window);
                break;
            }
        }
    }
    return -1;
}

void Selector::update_screen(sf::RenderWindow &window) {
    window.clear(sf::Color::White);
    window.draw(server_rect);
    window.draw(client_rect);
    window.draw(server_text);
    window.draw(client_text);
    window.display();
}

bool Selector::init_text() {
    if (!font.loadFromFile(FONT_FAMILY)) {
        return false;
    }
    server_text.setFont(font);
    server_text.setString("Server");
    server_text.setCharacterSize(60);
    server_text.setColor(sf::Color::Black);

    client_text.setFont(font);
    client_text.setString("Client");
    client_text.setCharacterSize(60);
    client_text.setColor(sf::Color::Black);

    sf::FloatRect r = server_text.getGlobalBounds();
    server_text.setOrigin(r.width / 2.0f, r.height / 2.0f);
    r = client_text.getGlobalBounds();
    client_text.setOrigin(r.width / 2.0f, r.height / 2.0f);

    server_text.setPosition(S_WIDTH / 2.0f, S_HEIGHT / 2.0f + 60.0f);
    client_text.setPosition(S_WIDTH / 2.0f, S_HEIGHT / 2.0f - 60.0f);
    return true;
}

void Selector::init_rect() {
    sf::FloatRect r = server_text.getGlobalBounds();
    server_rect.setSize(sf::Vector2f(r.width + 10.0f, r.height + 10.0f));
    r = client_text.getGlobalBounds();
    client_rect.setSize(sf::Vector2f(r.width + 10.0f, r.height + 10.0f));

    server_rect.setFillColor(sf::Color::White);
    server_rect.setOutlineThickness(2.0f);
    server_rect.setOutlineColor(sf::Color::Black);
    client_rect.setFillColor(sf::Color::White);
    client_rect.setOutlineThickness(2.0f);
    client_rect.setOutlineColor(sf::Color::Black);

    server_rect.setOrigin(server_rect.getSize().x / 2.0f, server_rect.getSize().y / 2.0f);
    client_rect.setOrigin(client_rect.getSize().x / 2.0f, client_rect.getSize().y / 2.0f);

    server_rect.setPosition(server_text.getPosition());
    client_rect.setPosition(client_text.getPosition());

    server_rect.move(2.0f, 20.0f);
    client_rect.move(2.0f, 20.0f);
}
