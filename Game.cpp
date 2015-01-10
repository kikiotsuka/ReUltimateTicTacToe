#include "Game.h"

Game::Game(sf::TcpSocket &socket, int mode, std::string username, std::string other) {
    this->socket = &socket;
    this->mode = mode;
    this->username = username;
    this->other = other;
}

int Game::run(sf::RenderWindow &window) {
    if (!init()){
        return -1;
    }

    establish_opening();
    reset_game();

    sf::Thread opponent(&Game::opponent_move, this);

    while (window.isOpen()) {
        if (disconnect) {
            //TODO do something if other player disconnects
            std::cout << "DISCONNECTED" << "\n";
        }
        if (gameover) {
            std::cout << "Player: " << winner << " won!" << "\n";
        }
        update_screen(window);

        if (!disconnect && !player_turn && !pending_move) {
            pending_move = true;
            opponent.launch();
        }

        sf::Event e;
        while (window.pollEvent(e)) {
            switch (e.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (e.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                break;
            case sf::Event::MouseButtonPressed:
                if (!disconnect && player_turn && e.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f coord(e.mouseButton.x, e.mouseButton.y);
                    mouse_click(coord);
                }
                break;
            case sf::Event::MouseMoved:
                sf::Vector2f coord(e.mouseMove.x, e.mouseMove.y);
                if (!disconnect && player_turn) {
                    mouse_move(coord);
                }
            }
        }
    }
    opponent.terminate();
    return -1;
}

void Game::opponent_move() {
    sf::Packet packet;
    if (socket->receive(packet) != sf::TcpSocket::Done) {
        disconnect = true;
        return;
    }
    sf::Int32 i, j, k, l;
    packet >> i >> j >> k >> l;
    grid[i][j][k][l] = enemy_piece;
    next_spot = sf::Vector2i(k, l);
    if (master_grid[k][l] != '.') {
        next_spot = sf::Vector2i(-1, -1);
    }
    if (check_win(grid[i][j], enemy_piece)) {
        master_grid[i][j] = enemy_piece;
        next_spot = sf::Vector2i(-1, -1);
        if (check_win(master_grid, enemy_piece)) {
            winner = enemy_piece;
            gameover = true;
        }
    }
    sf::Mutex mutex;
    mutex.lock();
    player_turn = true;
    pending_move = false;
    mutex.unlock();
}

void Game::establish_opening() {
    srand(time(NULL));
    if (mode == MODE_SERVER) {
        //randomize first turn
        if (rand() % 2 == 0) {
            player_piece = 'x';
            enemy_piece = 'o';
            player_turn = true;
        } else {
            player_piece = 'o';
            enemy_piece = 'x';
            player_turn = false;
        }
        sf::Packet p;
        sf::Int32 ep((int) enemy_piece);
        sf::Int32 pp((int) player_piece);
        p << ep << pp;
        if (socket->send(p) == sf::TcpSocket::Disconnected) {
            disconnect = true;
            return;
        }
    } else {
        sf::Packet p;
        if (socket->receive(p) != sf::TcpSocket::Done) {
            disconnect = true;
            return;
        }
        //note on receiving side pieces are switched
        sf::Int32 pp, ep;
        p >> pp >> ep;
        player_piece = (char) pp;
        enemy_piece = (char) ep;
        if (player_piece == 'x') {
            player_turn = true;
        } else {
            player_turn = false;
        }
    }
}

bool Game::valid_placement(int i, int j, int k, int l) {
    if (grid[i][j][k][l] != '.' || master_grid[i][j] != '.') return false;
    return sf::Vector2i(i, j) == next_spot || next_spot == sf::Vector2i(-1, -1);
}

bool Game::locate_square(int &i, int &j, int &k, int &l, sf::Vector2f coord) {
    for (i = 0; i < grid_rect.size(); i++) {
        for (j = 0; j < grid_rect[i].size(); j++) {
            for (k = 0; k < grid_rect[i][j].size(); k++) {
                for (l = 0; l < grid_rect[i][j][k].size(); l++) {
                    if (grid_rect[i][j][k][l].getGlobalBounds().contains(coord)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void Game::mouse_move(sf::Vector2f coord) {
    int i, j, k, l;
    if (locate_square(i, j, k, l, coord) && valid_placement(i, j, k, l)) {
        ghost_spot = new sf::Vector2f(grid_rect[i][j][k][l].getPosition());
    } else {
        delete ghost_spot;
        ghost_spot = NULL;
    }
}

void Game::mouse_click(sf::Vector2f coord) {
    int i, j, k, l;
    if (locate_square(i, j, k, l, coord)) {
        if (valid_placement(i, j, k, l)) {
            //delete ghost piece
            delete ghost_spot;
            ghost_spot = NULL;

            grid[i][j][k][l] = player_piece;
            next_spot = sf::Vector2i(k, l);
            if (master_grid[k][l] != '.') {
                next_spot = sf::Vector2i(-1, -1);
            }
            if (check_win(grid[i][j], player_piece)) {
                next_spot = sf::Vector2i(-1, -1);
                master_grid[i][j] = player_piece;
                if (check_win(master_grid, player_piece)) {
                    gameover = true;
                    winner = player_piece;
                }
            }
            sf::Packet p;
            sf::Int32 send_i(i), send_j(j), send_k(k), send_l(l);
            p << send_i << send_j << send_k << send_l;
            player_turn = false;
            if (socket->send(p) == sf::TcpSocket::Disconnected) {
                disconnect = true;
                return;
            }
        }
    }
}

void Game::reset_game() {
    char tmp = player_piece;
    player_piece = enemy_piece;
    enemy_piece = tmp;
    if (player_piece == 'x') {
        player_turn = true;
    } else {
        player_turn = false;
    }
    winner = '.';
    next_spot = sf::Vector2i(-1, -1);
    ghost_spot = NULL;
    pending_move = false;
    gameover = false;
    grid.clear();
    master_grid.clear();
    for (int i = 0; i < 3; i++) {
        std::vector<char> master_grid_row;
        for (int j = 0; j < 3; j++) {
            master_grid_row.push_back('.');
        }
        master_grid.push_back(master_grid_row);
    }
    for (int i = 0; i < 3; i++) {
        std::vector<std::vector<std::vector<char> > > large_grid_row;
        for (int j = 0; j < 3; j++) {
            std::vector<std::vector<char> > small_grid;
            for (int k = 0; k < 3; k++) {
                std::vector<char> small_grid_row;
                for (int l = 0; l < 3; l++) {
                    small_grid_row.push_back('.');
                }
                small_grid.push_back(small_grid_row);
            }
            large_grid_row.push_back(small_grid);
        }
        grid.push_back(large_grid_row);
    }
}

bool Game::check_win(std::vector<std::vector<char> > to_check, char piece) {
    if (check_vert(to_check, piece) || check_horz(to_check, piece)) return true;
    return check_majdiag(to_check, piece) || check_mindiag(to_check, piece);
}

bool Game::check_vert(std::vector<std::vector<char> > to_check, char piece) {
    for (int i = 0; i < to_check.size(); i++) {
        for (int j = 0; j < to_check.size(); j++) {
            if (to_check[j][i] != piece) {
                break;
            }
            if (j == to_check.size() - 1) return true;
        }
    }
    return false;
}

bool Game::check_horz(std::vector<std::vector<char> > to_check, char piece) {
    for (int i = 0; i < to_check.size(); i++) {
        for (int j = 0; j < to_check.size(); j++) {
            if (to_check[i][j] != piece) {
                break;
            }
            if (j == to_check.size() - 1) return true;
        }
    }
    return false;
}

bool Game::check_majdiag(std::vector<std::vector<char> > to_check, char piece) {
    for (int i = 0; i < to_check.size(); i++) {
        if (to_check[i][i] != piece) {
            return false;
        }
    }
    return true;
}

bool Game::check_mindiag(std::vector<std::vector<char> > to_check, char piece) {
    for (int i = 0; i < to_check.size(); i++) {
        if (to_check[i][to_check.size() - i - 1] != piece) {
            return false;
        }
    }
    return true;
}


bool Game::init() {
    if (!init_font()) {
        return false;
    }
    if (!init_texture()) {
        return false;
    }
    init_rect();
    disconnect = false;
    return true;
}

bool Game::init_font() {
    if (!font.loadFromFile(FONT_FAMILY)) {
        return false;
    }
    std::cout << username << " " << other << "\n";
    enemy_name.setFont(font);
    enemy_name.setString(other);
    enemy_name.setCharacterSize(NAME_HEIGHT);
    enemy_name.setPosition(620.0f, 20.0f);
    enemy_name.setColor(sf::Color::Black);
    player_name.setFont(font);
    player_name.setString(username);
    player_name.setCharacterSize(NAME_HEIGHT);
    player_name.setPosition(620.0f, S_HEIGHT - 20.0f - NAME_HEIGHT);
    player_name.setColor(sf::Color::Black);
    return true;
}

bool Game::init_texture() {
    if (!piece_texture.loadFromFile("piece.png")) {
        return false;
    }
    if (!piece_large_texture.loadFromFile("piece_large.png")) {
        return false;
    }
    if (!loading_texture.loadFromFile("loading_icon.jpg")) {
        return false;
    }
    circle.setTexture(piece_texture);
    circle.setTextureRect(sf::IntRect(PIECE_DIM, 0, PIECE_DIM, PIECE_DIM));
    x.setTexture(piece_texture);
    x.setTextureRect(sf::IntRect(0, 0, PIECE_DIM, PIECE_DIM));

    circle_large.setTexture(piece_large_texture);
    circle_large.setTextureRect(sf::IntRect(PIECE_DIM_LARGE, 0, PIECE_DIM_LARGE, PIECE_DIM_LARGE));
    x_large.setTexture(piece_large_texture);
    x_large.setTextureRect(sf::IntRect(0, 0, PIECE_DIM_LARGE, PIECE_DIM_LARGE));

    cat.setTexture(loading_texture);
    cat.setOrigin(cat.getGlobalBounds().width / 2.0f, cat.getGlobalBounds().height / 2.0f);
    cat.setPosition(S_WIDTH - 200.0f / 2.0f, S_HEIGHT / 2.0f);
    return true;
}

void Game::init_rect() {
    sf::FloatRect pfrect(player_name.getGlobalBounds());
    name_rects.push_back(sf::RectangleShape(sf::Vector2f(pfrect.width + 10.0f, pfrect.height+ 10.0f)));
    pfrect = sf::FloatRect(enemy_name.getGlobalBounds());
    name_rects.push_back(sf::RectangleShape(sf::Vector2f(pfrect.width + 10.0f, pfrect.height+ 10.0f)));
    name_rects[0].setPosition(enemy_name.getPosition());
    name_rects[1].setPosition(player_name.getPosition());

    //initialize tile system
    float large_buffer = 10.0f;
    float small_buffer = 2.0f;
    float ninth = 580.0f / 9.0f - 5.0f;
    float third = 580.0f / 3.0f - 5.0f;

    sf::RectangleShape r_small(sf::Vector2f(ninth, ninth));
    sf::RectangleShape r_large(sf::Vector2f(third + small_buffer * 3, third + small_buffer * 3));
    r_small.setFillColor(sf::Color::White);
    r_large.setFillColor(sf::Color::Red);
    r_small.setOutlineColor(sf::Color::Black);
    r_small.setOutlineThickness(-1);
    r_large.setOutlineColor(sf::Color::Black);
    r_large.setOutlineThickness(-1);

    for (int i = 0; i < 3; i++) {
        std::vector<std::vector<std::vector<sf::RectangleShape> > > large_tile_grid_row;
        std::vector<sf::RectangleShape> master_grid_row;
        for (int j = 0; j < 3; j++) {
            std::vector<std::vector<sf::RectangleShape> > large_tile_grid;
            for (int k = 0; k < 3; k++) {
                std::vector<sf::RectangleShape> small_tile_grid_row;
                for (int l = 0; l < 3; l++) {
                    r_small.setPosition(third * j + large_buffer * (j + 1) + (small_buffer + ninth) * l,
                                        third * i + large_buffer * (i + 1) + (small_buffer + ninth) * k);
                    small_tile_grid_row.push_back(sf::RectangleShape(r_small));
                }
                large_tile_grid.push_back(small_tile_grid_row);
            }
            large_tile_grid_row.push_back(large_tile_grid);
            //master grid
            r_large.setPosition(third * j + large_buffer * (j + 1) - large_buffer * 0.5f,
                                third * i + large_buffer * (i + 1) - large_buffer * 0.5f);
            master_grid_row.push_back(sf::RectangleShape(r_large));
        }
        grid_rect.push_back(large_tile_grid_row);
        master_grid_rect.push_back(master_grid_row);
    }
}

void Game::update_screen(sf::RenderWindow &window) {
    window.clear(sf::Color::White);
    for (int i = 0; i < name_rects.size(); i++) {
        window.draw(name_rects[i]);
    }
    window.draw(player_name);
    window.draw(enemy_name);
    for (int i = 0; i < master_grid_rect.size(); i++) {
        for (int j = 0; j < master_grid_rect[i].size(); j++) {
            if (!player_turn) {
                master_grid_rect[i][j].setFillColor(sf::Color::White);
            } else if (next_spot == sf::Vector2i(-1, -1) && master_grid[i][j] == '.') {
                master_grid_rect[i][j].setFillColor(sf::Color::Red);
            } else if (sf::Vector2i(i, j) == next_spot){
                master_grid_rect[i][j].setFillColor(sf::Color::Red);
            } else {
                master_grid_rect[i][j].setFillColor(sf::Color::White);
            }
            window.draw(master_grid_rect[i][j]);
        }
    }
    for (int i = 0; i < grid_rect.size(); i++) {
        for (int j = 0; j < grid_rect[i].size(); j++) {
            for (int k = 0; k < grid_rect[i][j].size(); k++) {
                for (int l = 0; l < grid_rect[i][j][k].size(); l++) {
                    window.draw(grid_rect[i][j][k][l]);
                    if (grid[i][j][k][l] == 'o') {
                        circle.setPosition(grid_rect[i][j][k][l].getPosition());
                        circle.setColor(sf::Color(255, 255, 255, 255));
                        window.draw(circle);
                    } else if (grid[i][j][k][l] == 'x') {
                        x.setPosition(grid_rect[i][j][k][l].getPosition());
                        x.setColor(sf::Color(255, 255, 255, 255));
                        window.draw(x);
                    }
                }
            }
        }
    }
    for (int i = 0; i < master_grid_rect.size(); i++) {
        for (int j = 0; j < master_grid_rect[i].size(); j++) {
            if (master_grid[i][j] == 'o') {
                circle_large.setPosition(master_grid_rect[i][j].getPosition());
                circle.setColor(sf::Color(255, 255, 255, 255));
                window.draw(circle_large);
            } else if (master_grid[i][j] == 'x') {
                x_large.setPosition(master_grid_rect[i][j].getPosition());
                x_large.setColor(sf::Color(255, 255, 255, 255));
                window.draw(x_large);
            }
        }
    }
    if (ghost_spot) {
        if (player_piece == 'o') {
            circle.setPosition(*ghost_spot);
            circle.setColor(sf::Color(255, 255, 255, 128));
            window.draw(circle);
        } else {
            x.setPosition(*ghost_spot);
            x.setColor(sf::Color(255, 255, 255, 128));
            window.draw(x);
        }
    }
    window.display();
}
