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

    
   
    void cb_reset();
    void cb_quit();


    //knapper
    Button reset_button;  
    Button quit_button;
};