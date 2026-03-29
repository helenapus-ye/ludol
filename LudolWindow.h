#pragma once

#include "AnimationWindow.h"
#include "widgets/Button.h"
#include <vector>
#include <fstream>
#include <stdexcept>
#include "player.h"

using namespace TDT4102;


class LudolWindow : public AnimationWindow {
public:

    //funskjonen som lager viduet. 
    //X-coordinate of the window. Y-coordinate of the window. 
    ///Width of the window, Height of the window, Title of the window.
    LudolWindow(int x, int y, int width, int height, const std::string& title);

    //må lages
    void play();

    

private:

    //funskjoner å lage

    //lager ludobrette
    void draw_board();

    //tegner brikkene
    void draw_players( std::vector<Player> players);

    void draw_piece(Piece piece, Player player);

    void draw_infoText(std::string info); //tegner tekst over spillbrett med instruksjoner 

    //håndterer drag n drop av brikker
    void check_drag_n_drop();
    void draw_dragged_piece();
    int find_piece_at(const int x, const int y);
    void handle_drop(const int x, const int y);

    //Tegner brikke i skjermkoordinater
    void draw_piece(const int x, const int y, const TDT4102::Color color, const bool isDraggedOriginal);


    //tegner antall spy og drikke osv
    void draw_poeng();

    /**
     * @brief Handles mouse click events to place marks.
     * @param x X-coordinate of the click.
     * @param y Y-coordinate of the click.
     */
    void handle_click(int x, int y);

    /**
     * @brief Checks if there is a winner.
     * @return true if a player has won, false otherwise.
     */
    bool check_winner();

    

    /**
     * @brief Resets the game to its initial state.
     */
    void reset_game();

    /**
     * @brief Writes the game result to a file.
     * @param result The result string to write.
     */
    void write_result_to_file(const std::string& result);

    void roll_dice();

    void flytt_brike(int valgtBrikkeIndex, int steps_made);

    std::vector<Player> players;

    int current_player_index = 0; // Index til hvilken spiller som har tur (0-3)
    int dice_result = 0;
    std::string info;//tekst som skal vises over brettet

    //knapper
    Button reset_button;  
    Button quit_button;
    Button dice_button;

    int dragging_piece_index = -1;  //0-15 hvilken brikke som dras (player_id * brikk_id), -1 = ingen
    int drag_x = 0, drag_y = 0;    // museposisjon under drag
};