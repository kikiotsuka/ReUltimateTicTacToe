#include "Input.h"

Input::Input(std::string &input_str, std::string query_message) {
    this->input_str = &input_str;
    this->query_message = query_message;
    init_rect();
}

int Input::run(sf::RenderWindow &window) {
    if (!init_text(query_message)) {
        return -1;
    }
    update_screen(window);

    std::string input = "";
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            switch(e.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::TextEntered:
                if (e.text.unicode == 8) { //backspace
                    input = input.substr(0, input.size() - 1);
                    input_text.setString(input);
                    update_cursor_position();
                    update_screen(window);
                } else if (e.text.unicode == 13) { //carriage return
                    if (input.size() > 0) {
                        *input_str = input;
                        if (query_message == "Enter server ip") {
                            return SCREEN_CONNECTION_WAITING;
                        }
                        return SCREEN_TYPE_SELECTOR;
                    }
                } else if (e.text.unicode == 27) { //escape key
                    window.close();
                } else if (input.size() < 20 && validate_keystroke(e.text.unicode)) {
                    char to_add = static_cast<char>(e.text.unicode);
                    input += to_add;
                    input_text.setString(input);
                    update_cursor_position();
                    update_screen(window);
                }
                break;
            }
        }
    }
    return -1;
}

void Input::init_rect() {
    input_rect.setSize(sf::Vector2f(S_WIDTH * 0.565f, 35.0f));
    input_rect.setOutlineThickness(-1);
    input_rect.setOutlineColor(sf::Color::Black);
    input_rect.setPosition(S_WIDTH / 2.0f - input_rect.getSize().x / 2.0f,
        S_HEIGHT / 2.0f - input_rect.getSize().y / 2.0f);

    cursor.setSize(sf::Vector2f(2.0f, 25.0f));
    cursor.setOrigin(0, cursor.getSize().y / 2.0f);
    cursor.setFillColor(sf::Color::Black);
}

bool Input::init_text(std::string message) {
    if (!font.loadFromFile(FONT_FAMILY)) {
        return false;
    }
    input_text.setFont(font);
    input_text.setCharacterSize(25);
    input_text.setColor(sf::Color::Black);
    input_text.setString(*input_str);

    query_text.setFont(font);
    query_text.setCharacterSize(25);
    query_text.setColor(sf::Color::Black);
    query_text.setString(message);

    input_text.setOrigin(0, 25.0f / 2.0f);
    input_text.setPosition(input_rect.getPosition().x + 5.0f, S_HEIGHT / 2.0f);
    query_text.setPosition(input_rect.getPosition().x, input_rect.getPosition().y - 30.0f);
    update_cursor_position();

    return true;
}

void Input::update_cursor_position() {
    sf::Vector2f coord = input_text.getPosition();
    coord.x += input_text.getGlobalBounds().width;
    cursor.setPosition(coord);
}

bool Input::validate_keystroke(int ascii_id) {
    if (ascii_id >= 'A' && ascii_id <= 'Z') return true;
    if (ascii_id >= '0' && ascii_id <= '9') return true;
    return ascii_id >= 'a' && ascii_id <= 'z' || ascii_id == '.';
}

void Input::update_screen(sf::RenderWindow &window) {
    window.clear(sf::Color::White);
    window.draw(input_rect);
    window.draw(cursor);
    window.draw(input_text);
    window.draw(query_text);
    window.display();
}
