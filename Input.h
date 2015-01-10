#ifndef INPUT_H
#define INPUT_H

#include <string>
#include "Screen.h"
#include "Meta.h"

class Input : public Screen {
private:
    std::string *input_str, query_message;
    sf::RectangleShape input_rect, cursor;
    sf::Font font;
    sf::Text input_text, query_text;
    int max_char;
    void init_rect();
    bool init_text(std::string message);
    void update_cursor_position();
    bool validate_keystroke(int ascii_id);
    void update_screen(sf::RenderWindow &window);
public:
    Input(std::string &input_str, std::string query_message, int max_char);
    virtual int run(sf::RenderWindow &window);
};

#endif // INPUT_H