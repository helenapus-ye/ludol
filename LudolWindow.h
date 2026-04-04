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

    void draw_infoText(); //tegner tekst over spillbrett med instruksjoner 

    //håndterer drag n drop av brikker
    void check_drag_n_drop();
    void draw_dragged_piece();
    int find_piece_at(const int x, const int y);
    void handle_drop(const int x, const int y);

    //regner ut skjermpositionen til en brikke basert på om den er i start, på brettet eller i mål
    std::pair<int, int> calculate_screen_position(const int player_index, const Piece piece);
    std::pair<int, int> calculate_piece_board_position(const int player_index, const Piece piece);
    std::pair<int, int> calculate_new_board_position(const int player_index, const Piece piece, const int steps);

    

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

    void updateAfterMove();

    void invalidMove(Player& personWhoFailed, Piece& deadPiece);

    bool has_pieces_on_board(int player_index); //returner true om spiller har noen brikker på brette

    void flytt_brike(int valgtBrikkeIndex, int steps_made);
    Piece flytt_brike_struct(Piece piece, const int steps_to_make);

    void player_spydde(int player_index);

    enum class GameWaitState {
        WaitingForRoll,
        WaiutingForMove,
    };

    GameWaitState state = GameWaitState::WaitingForRoll;

    bool skip_info_update = false; //Betstemmer om info teksten i play() skal endres eller ikke

    std::vector<Player> players;

    int current_player_index = 0; // Index til hvilken spiller som har tur (0-3)
    int dice_result = 0;
    int tryNr = 0; //antall ganger spilleren har kastet terningen på sin tur
    std::string info;//tekst som skal vises over brettet
    std::string feil;//tekst som skal vises over brettet
    std::string move_error; //for teksten fra runtime_error throws (f.eks. "Du må flytte det antallet steg du kastet på terningen!")
    std::string drink_message; //for teksten fra invalidMove

    //knapper
    Button reset_button;  
    Button quit_button;
    Button dice_button;

    //spydd knapper
    Button spydd_button_0;
    Button spydd_button_1;
    Button spydd_button_2;
    Button spydd_button_3;

    int dragging_piece_index = -1;  //0-15 hvilken brikke som dras (player_id * brikk_id), -1 = ingen
    int drag_x = 0, drag_y = 0;    // museposisjon under drag
};