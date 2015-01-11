#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include "Screen.h"
#include "Meta.h"

class Game : public Screen {
private:
    sf::TcpSocket *socket;
    int mode;
    std::string username, other;

    //for display
    sf::Font font;
    sf::Text player_name, enemy_name;
    sf::Text win_text, lose_text;
    sf::Text disconnect_text, rematch_text;
    sf::Texture piece_texture, piece_large_texture;
    sf::Texture loading_texture;
    sf::Sprite circle, x, circle_large, x_large, cat;
    sf::RectangleShape player_turn_rect, enemy_turn_rect;
    sf::RectangleShape disconnect_rect, rematch_rect;

    //hold vars
    std::vector<std::vector<std::vector<std::vector<char> > > > grid;
    std::vector<std::vector<char> > master_grid;
    std::vector<std::vector<std::vector<std::vector<sf::RectangleShape> > > > grid_rect;
    std::vector<std::vector<sf::RectangleShape> > master_grid_rect;
    bool player_turn; //keep track of who's move
    bool pending_move; //prevent thread from being called multiple times
    char player_piece, enemy_piece;
    bool gameover;

    bool disconnect; //in case of bad stuff

    sf::Vector2i next_spot; //where you can place the next piece
    sf::Vector2f *ghost_spot; //preview location

    void opponent_move();
    void establish_opening(); //pick who goes first, piece assignment

    bool valid_placement(int i, int j, int k, int l);
    bool locate_square(int &i, int &j, int &k, int &l, sf::Vector2f coord);

    void mouse_move(sf::Vector2f coord);
    void mouse_click(sf::Vector2f coord);
    void mouse_left();

    bool init();
    bool init_font();
    bool init_texture();
    void init_rect();

    //check for a win
    bool check_win(std::vector<std::vector<char> > to_check, char piece);
    bool check_vert(std::vector<std::vector<char> > to_check, char piece);
    bool check_horz(std::vector<std::vector<char> > to_check, char piece);
    bool check_majdiag(std::vector<std::vector<char> > to_check, char piece);
    bool check_mindiag(std::vector<std::vector<char> > to_check, char piece);

    void update_screen(sf::RenderWindow &window);
    void reset_game();
public:
    Game(sf::TcpSocket &socket, int mode,
         std::string username, std::string other);
    virtual int run(sf::RenderWindow &window);
};

#endif // GAME_H