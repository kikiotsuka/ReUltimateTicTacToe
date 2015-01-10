#ifndef SELECTOR_H
#define SELECTOR_H

#include <SFML/Graphics.hpp>
#include "Screen.h"
#include "Meta.h"

class Selector : public Screen {
private:
    int *mode;
    sf::RectangleShape server_rect, client_rect;
    sf::Font font;
    sf::Text server_text, client_text;
    void update_screen(sf::RenderWindow &window);
    bool init_text();
    void init_rect();
public:
    Selector(int &mode);
    int run(sf::RenderWindow &window);
};

#endif // SELECTOR_H