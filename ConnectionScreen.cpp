#include "ConnectionScreen.h"

ConnectionScreen::ConnectionScreen(sf::TcpSocket &socket, int mode, std::string ip,
                 std::string username, std::string &other) {
    srand(time(NULL));
    this->socket = &socket;
    this->mode = mode;
    this->ip = ip;
    this->username = username;
    this->other = &other;
    connected = false;
}

int ConnectionScreen::run(sf::RenderWindow &window) {
    if (!init_text()) {
        return -1;
    }
    if (!init_img()) {
        return -1;
    }
    if (mode == MODE_CLIENT) {
        display_text = "Connecting to " + ip;
    } else {
        display_text = "Waiting for client to connect";
    }
    display_text += ". Please wait...";
    update_text(text, display_text, sf::Vector2f(S_WIDTH / 2.0f, S_HEIGHT / 2.0f - 105.0f));
    sf::Thread connect(&ConnectionScreen::connect_to_other, this);
    connect.launch();
    while (window.isOpen()) {
        sprite.setRotation(sprite.getRotation() + 1.0f);
        window.clear(sf::Color::White);
        window.draw(text);
        window.draw(funny_text);
        window.draw(sprite);
        window.display();

        if (connected) {
            return SCREEN_PLAYING;
        }

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            } else if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }
    }
    connect.terminate();
    return -1;
}

void ConnectionScreen::connect_to_other() {
    int do_stuff = 2 + rand() % 3;
    //force user to appreciate my cat waiting screen animation
    //enforce the illusion that the program is doing something meaningful
    //http://steins-gate.wikia.com/wiki/Attractor_Field
    for (int i = 0; i < do_stuff; i++) {
        if (username != "mitsuru") {
            sf::sleep(sf::seconds(1.048596));
        }
    }
    display_text = "Connection established! Retrieving data from ";
    if (mode == MODE_SERVER) {
        listener.listen(SERVER_PORT);
        listener.accept(*socket);
        display_text += "client";
    } else {
        socket->connect(ip, SERVER_PORT);
        display_text += "server";
    }
    update_text(text, display_text, sf::Vector2f(S_WIDTH / 2.0f, S_HEIGHT / 2.0f - 105.0f));
    //send username and receive username
    sf::Packet packet;
    packet << username;
    socket->send(packet);
    packet.clear();
    socket->receive(packet);
    packet >> *other;
    do_stuff = 3 + rand() % 4;
    for (int i = 0; i < do_stuff; i++) {
        if (username != "mitsuru") {
            sf::sleep(sf::seconds(1.048596));
        }
    }
    display_text = "Initializing game vars. The game will start momentarily";
    update_text(text, display_text, sf::Vector2f(S_WIDTH / 2.0f, S_HEIGHT / 2.0f - 105.0f));
    do_stuff = 2 + rand() % 3;
    for (int i = 0; i < do_stuff; i++) {
        if (username != "mitsuru") {
            sf::sleep(sf::seconds(1.048596));
        }
    }
    connected = true;
}

bool ConnectionScreen::init_text() {
    if (!font.loadFromFile(FONT_FAMILY)) {
        return false;
    }
    text.setFont(font);
    text.setCharacterSize(25);
    text.setColor(sf::Color::Black);
    update_text(text, display_text, sf::Vector2f(S_WIDTH / 2.0f, S_HEIGHT / 2.0f - 105.0f));

    std::vector<std::string> quotes;
    quotes.push_back("Grooming cats");
    quotes.push_back("Generating world line 1.048596");
    quotes.push_back("Meow meow meow");
    quotes.push_back("Deleting system32");
    quotes.push_back("$rm -rf *");
    quotes.push_back("Cat > Dog");
    quotes.push_back("<Queue nyan cat music>");
    troll_quotes = quotes[rand() % quotes.size()];

    funny_text.setFont(font);
    funny_text.setCharacterSize(25);
    funny_text.setColor(sf::Color::Black);
    update_text(funny_text, troll_quotes, sf::Vector2f(S_WIDTH / 2.0f, S_HEIGHT / 2.0f + 105.0f));
    return true;
}

bool ConnectionScreen::init_img() {
    if (!texture.loadFromFile("loading_icon.jpg")) {
        return -1;
    }
    sprite.setTexture(texture);
    sprite.setOrigin(sprite.getGlobalBounds().width / 2.0f,
                     sprite.getGlobalBounds().height / 2.0f);
    sprite.setPosition(S_WIDTH / 2.0f, S_HEIGHT / 2.0f);
    return true;
}

void ConnectionScreen::update_text(sf::Text &text, std::string str, sf::Vector2f pos) {
    text.setString(str);
    text.setOrigin(text.getGlobalBounds().width / 2.0f, text.getGlobalBounds().height / 2.0f);
    text.setPosition(pos);
}
