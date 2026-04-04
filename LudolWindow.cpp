#include "LudolWindow.h"
#include <iostream>
#include <functional>

constexpr int BOARD_ROWS_N_COLUMNS = 15;

/// kordinater for brettet, ligger visualisert i brett kordinater.png
// I toppen av LudolWindow.cpp
constexpr int BOARD_X = 30;
constexpr int BOARD_Y = 60;
constexpr int BOARD_SIZE = 600;
constexpr int CELL = BOARD_SIZE / BOARD_ROWS_N_COLUMNS;

// Høyre panel (til høyre for brettet)
constexpr int PANEL_X = BOARD_X + BOARD_SIZE + 40;  // start av høyre panel
constexpr int PANEL_Y = 60;                          // topp av høyre panel
constexpr int PANEL_W = 600;                          // bredde på høyre panel


// Alle 52 baneceller i spillrekkefølge {rad, kol}
// Gul starter på index 0, ruter rundt brettet med klokken
const std::vector<std::pair<int,int>> BOARD_PATH = {
    {0,6},{1,6},{2,6},{3,6},{4,6},{5,6}, //gul under
    {6,5},{6,4},{6,3},{6,2},{6,1},{6,0}, //gul opp
    {7,0},                                 //TOPP
    {8,0},{8,1},{8,2},{8,3},{8,4},{8,5}, //blå nedover
    {9,6},{10,6},{11,6},{12,6},{13,6},{14,6}, //blå under
    {14,7},                    //høyre
    {14,8},{13,8},{12,8},{11,8},{10,8},{9,8}, //rød over
    {8,9},{8,10},{8,11},{8,12},{8,13},{8,14}, //rød venstre     
    {7,14}, //under
    {6,14},{6,13},{6,12},{6,11},{6,10},{6,9},  //høyre grønn
    {5,8},{4,8},{3,8},{2,8},{1,8},{0,8},  //over grønn
    {0,7} //venstre
};

const std::vector<TDT4102::Color> COLORS = {
    TDT4102::Color::yellow,
    TDT4102::Color::blue,
    TDT4102::Color::red,
    TDT4102::Color::green,
};

const std::vector<std::vector<std::pair<float,float>>> HOME_START = {
    {{1.5,1.5},{3.5,1.5},{1.5,3.5},{3.5,3.5}},
    {{10.5, 1.5},{12.5,1.5},{10.5,3.5},{12.5,3.5}},
    {{10.5,10.5},{12.5,10.5},{10.5,12.5},{12.5,12.5}},
    {{1.5,10.5},{3.5,10.5},{1.5,12.5},{3.5,12.5}}
};

const std::vector<std::vector<std::pair<int, int>>> HOME_END {
    {{1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}},
    {{7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}, {7, 6}},
    {{13, 7}, {12, 7}, {11, 7}, {10, 7}, {9, 7}, {8, 7}},
    {{7, 13}, {7, 12}, {7, 11}, {7, 10}, {7, 9}, {7, 8} }
};

LudolWindow::LudolWindow(int x, int y, int width, int height, const std::string &title)
    : AnimationWindow(x, y, width, height, title),
      reset_button({10, 10}, 100, 30, "Reset"),
      quit_button({width - 110, 10}, 100, 30, "Quit"),
      dice_button({PANEL_X, PANEL_Y + 100}, 200, 50, "Kast terning"),
      spydd_button_0({PANEL_X + 400, PANEL_Y + 300}, 120, 30, "Spiller spydde"),
      spydd_button_1({PANEL_X + 400, PANEL_Y + 340}, 120, 30, "Spiller spydde"),
      spydd_button_2({PANEL_X + 400, PANEL_Y + 380}, 120, 30, "Spiller spydde"),
      spydd_button_3({PANEL_X + 400, PANEL_Y + 420}, 120, 30, "Spiller spydde")
{
    add(reset_button);
    add(quit_button);
    add(dice_button);
    add(spydd_button_0);
    add(spydd_button_1);
    add(spydd_button_2);
    add(spydd_button_3);

    reset_button.setCallback(std::bind(&LudolWindow::reset_game, this));
    quit_button.setCallback(std::bind(&LudolWindow::close, this));
    dice_button.setCallback(std::bind(&LudolWindow::roll_dice, this));

    spydd_button_0.setCallback([this]() { player_spydde(0); });
    spydd_button_1.setCallback([this]() { player_spydde(1); });
    spydd_button_2.setCallback([this]() { player_spydde(2); });
    spydd_button_3.setCallback([this]() { player_spydde(3); });
}

void LudolWindow::play()
{
    //Lager nye spillere og setter start posisijoner
   reset_game();

    // logger informasjonen om spillerne og brikkene deres til terminalen
    for (const auto &player : players)
    {
        std::cout << "Spiller: " << player.name
                  << ", playernumber: " << player.playernumber
                  << ", drukket: " << player.antallDrukket
                  << ", spydd: " << player.antallSpydd << "\n";

        for (int i = 0; i < 4; ++i)
        {
            std::cout << "  Brikke " << i
                      << ": path_index=" << player.pieces[i].path_index
                      << ", home=" << player.pieces[i].home_start
                      << ", oneround=" << player.pieces[i].oneround << "\n";
        }
    }
    

    while (!should_close())
    {
        //om vi venter på kast
        if (state == GameWaitState::WaitingForRoll) {
            //og det ikke er slik at infoen skal skippe
            if (!skip_info_update) {
                //endre info til
                info = "Det er nå " + players.at(current_player_index).name + " sin tur. Trykk for å kaste terningen";
            }
        //om det er kastet og vi venter på move
        } else if (state == GameWaitState::WaiutingForMove) {
            //endre info til dette
            info = players.at(current_player_index).name + " kastet " + std::to_string(dice_result) + ". Dra en brikke til et felt å flytte til!";
        }

        try
        {
            check_drag_n_drop();
            draw_board();
            draw_infoText();
            draw_players(players);
            draw_poeng();
            draw_dragged_piece();
        }
        catch(const std::exception& e)
        {
            //Cath blokken
           move_error = e.what();
           dragging_piece_index = -1;

           if (state == GameWaitState::WaiutingForMove) {
                state = GameWaitState::WaitingForRoll; // gå tilbake til å vente på terningkast
           }
           //DRINK!!
           current_player_index = (current_player_index + 1) % 4; // bytt spiller ved feil
        }

        // tilslutt
        next_frame();
    }
}

void LudolWindow::draw_dragged_piece() {

    if (dragging_piece_index != -1) {

        const int dragging_piece_player_index = dragging_piece_index / players.size();

        draw_piece(drag_x, drag_y, players.at(dragging_piece_player_index).color, true);
    }
}

int LudolWindow::find_piece_at(const int x, const int y) {
    const int half = CELL / 2;

    for (const auto& player : players) {
        for (const auto& piece : player.pieces) {
            int px, py;

            if (piece.home_start) {
                const auto& pos = HOME_START.at(player.playernumber).at(piece.piece_number);
                px = BOARD_X + static_cast<int>(std::round(pos.first * CELL + CELL / 2.0));
                py = BOARD_Y + static_cast<int>(std::round(pos.second * CELL + CELL / 2.0));
            } else if (piece.home_end) {
                const int idx = std::min(
                    static_cast<int>(piece.steps_made - BOARD_PATH.size()),
                    static_cast<int>(HOME_END.at(player.playernumber).size() - 1)
                );
                const auto& pos = HOME_END.at(player.playernumber).at(idx);
                px = BOARD_X + pos.first * CELL + half;
                py = BOARD_Y + pos.second * CELL + half;
            } else {
                const auto& pos = BOARD_PATH.at(piece.path_index);
                px = BOARD_X + pos.first * CELL + half;
                py = BOARD_Y + pos.second * CELL + half;
            }

            // Firkant-hitbox rundt brikkens senter
            if (std::abs(x - px) <= half && std::abs(y - py) <= half) {
                return player.playernumber * 4 + piece.piece_number;
            }
        }
    }

    return -1; // ingen brikke funnet
}

std::pair<int, int> screen_to_board(const int x, const int y) {
    std::vector<std::pair<int, int>> result;
    const int half = CELL / 2;

    for (int i = 0; i < BOARD_ROWS_N_COLUMNS; ++i) {
        for (int j = 0; j < BOARD_ROWS_N_COLUMNS; ++j) {
            const int px = BOARD_X + i * CELL + half;
            const int py = BOARD_Y + j * CELL + half;

            if (std::abs(x - px) <= half && std::abs(y - py) <= half) {

                return {i, j};
            }
        }
    }
    //spiller flytter til sted utenfor
    throw std::runtime_error("DRIKK! Brikkene må være på brettet!");
}

std::pair<int, int> LudolWindow::calculate_screen_position(const int player_index, const Piece piece) {
    if (piece.home_start) {
        const std::pair<float, float> piecePos = HOME_START.at(player_index).at(piece.piece_number);
   
        const float x = piecePos.first;
        const float y = piecePos.second;

        return {BOARD_X + static_cast<int>(std::round((x * CELL)+CELL/2.0)), BOARD_Y+ static_cast<int>(std::round((y * CELL)+CELL/2.0))};
    }
    else if(piece.home_end){
        const int home_end_path_index = std::min(piece.steps_made - BOARD_PATH.size(), HOME_END.at(player_index).size() - 1);

        const std::pair<int, int> piecePos = HOME_END.at(player_index).at(home_end_path_index);

        return {(BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2};

    } else {
        const std::pair<int, int> piecePos = BOARD_PATH.at(piece.path_index);

        return {(BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2};
    }
}

std::pair<int, int> LudolWindow::calculate_piece_board_position(const int player_index, const Piece piece) {
    if (piece.home_start) {
        return HOME_START.at(player_index).at(piece.piece_number);
    }
    else if(piece.home_end){
        const int home_end_path_index = std::min(piece.steps_made - BOARD_PATH.size(), HOME_END.at(player_index).size() - 1);

        return HOME_END.at(player_index).at(home_end_path_index);

    } else {
        return BOARD_PATH.at(piece.path_index);
    }
   /* piece.home_start ? HOME_START.at(player_index).at(piece.piece_number) :
                          piece.home_end ? HOME_END.at(player_index).at(std::min(piece.steps_made - BOARD_PATH.size(), HOME_END.at(player_index).size() - 1)) :
                          BOARD_PATH.at(piece.path_index);*/
}

std::pair<int, int> LudolWindow::calculate_new_board_position(const int player_index, const Piece piece, const int steps) {


    Piece newPiece = flytt_brike_struct(piece, steps); // simulerer flytting av brikken for å finne ut hvor den havner uten å endre den faktiske brikken

    return calculate_piece_board_position(player_index, newPiece);


    Piece tempPiece = piece; // lager en midlertidig kopi av brikken for å simulere flytting uten å endre den faktiske brikken

   /*     
    //std::pair<int, int> newPos = calculate_piece_board_position(player_index, piece);
    int remaining_steps = steps;


    while (remaining_steps-- > 0)
    {
        tempPiece.path_index++;
        tempPiece.steps_made++;

        if (tempPiece.home_start) {
            tempPiece.home_start = false;            
        }
        else if(piece.home_end){

            const int home_end_path_index = std::min(piece.steps_made - BOARD_PATH.size(), HOME_END.at(player_index).size() - 1);

            const std::pair<int, int> piecePos = HOME_END.at(player_index).at(home_end_path_index);

            tempPiece.

            return {(BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2};

        } else {
            const std::pair<int, int> piecePos = BOARD_PATH.at(piece.path_index);

            return {(BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2};
        }
    }*/

}

//DEnne kalles når spilleren slipper en brikke (dragging_piece_index)

void LudolWindow::handle_drop(const int x, const int y) {

    if (state != GameWaitState::WaiutingForMove) {
        throw std::runtime_error("Du må kaste terningen før du kan flytte en brikke!");
    }
    
    if (dragging_piece_index == -1) return; // ingen brikke dras
    
    const int dragging_piece_player_index = dragging_piece_index / players.size();

    if (dragging_piece_player_index != current_player_index) {
        throw std::runtime_error("Du kan bare flytte dine egne brikker!");
    }

    const auto draggedPieceOriginal = players.at(dragging_piece_player_index).pieces.at(dragging_piece_index % 4);

    //ser om vi får gjort om screen kordinatene til en celle på brette.
    //om det ikke er mulig betyr det at spiller har dratt brikken utenfor brette og må drikke
    std::pair<int, int> droppedPieceBoardPos;
    try {
        droppedPieceBoardPos = screen_to_board(x, y);
    } catch (...) {
        invalidMove(players.at(current_player_index), players.at(current_player_index).pieces.at(dragging_piece_index % 4));
        throw;
    }
    
    //Sjekke om det er rokade på dette felte
    bool blocked = false;
    //setter en rar verdi så program ikke kræsjer
    int stepWhereBlooked = 999;

    //Vi tester om et, to .. seks mulige felt er gyldige, eller så thrower vi!
    for (int steps = 1; steps <= 6; steps++) { 

        //if we move draggedPieceOriginal and drop it after X steps, will it have the  
        const auto draggedPieceOriginalNextPos = calculate_new_board_position(dragging_piece_player_index, draggedPieceOriginal, steps);

        
        //Endrer blocked og stepWhereBlooked — bare fiendtlige rokader blokkerer
        //finner ut om det er rokade i løpet av de 6 neste stegene
        for (const auto &player : players) {
            if (player.playernumber == current_player_index) continue; // hopp over egne brikker
            for (const auto &piece : player.pieces) {
                if (piece.rokade) {
                    auto piecePos = calculate_new_board_position(player.playernumber, piece, 0);
                    if (piecePos.first == draggedPieceOriginalNextPos.first && piecePos.second == draggedPieceOriginalNextPos.second) {
                        blocked = true;
                        stepWhereBlooked = steps;
                    }
                }
            }
        }
        
        

        if (draggedPieceOriginalNextPos.first == droppedPieceBoardPos.first && draggedPieceOriginalNextPos.second == droppedPieceBoardPos.second) {
   
            //Inni her: Det GÅR ANN å komme hit med 1-6 flytt.

            // Brikker fra home_start — spesialregel: må ha 1/6 og plasseres på felt 1
            if (draggedPieceOriginal.home_start) {
                if (dice_result != 1 && dice_result != 6) {
                    invalidMove(players.at(current_player_index), players.at(current_player_index).pieces.at(dragging_piece_index % 4));
                    throw std::runtime_error("Du må kaste 1 eller 6 for å ta ut en brikke!");
                }
                if (steps != 1) {
                    //om brikken var på home start, kan den bare ta et steg
                    invalidMove(players.at(current_player_index), players.at(current_player_index).pieces.at(dragging_piece_index % 4));
                    throw std::runtime_error("Brikker fra start må plasseres på første felt!");
                }

                // Hopp over vanlig terning-sjekk, gå rett til landing-sjekk og flytt
            } else {
                // steps != (7-dice_result) er røros konversjonen (1=7-6, 2=7-5, 3=7-4, osv)
                //kast er gyldig om antall spes enten er dice_results eller 7-dice_result
                if (steps != dice_result && steps != (7 - dice_result)) {
                    invalidMove(players.at(current_player_index), players.at(current_player_index).pieces.at(dragging_piece_index % 4));
                    throw std::runtime_error("Du må flytte det antallet steg du kastet på terningen!");
                }
                
                //sjekker om det er en rokade og om spilleren prøver og gå over rokaden
                if (blocked && steps >= stepWhereBlooked) {
                    invalidMove(players.at(current_player_index), players.at(current_player_index).pieces.at(dragging_piece_index % 4));
                    throw std::runtime_error("Du kan ikke hoppe over en rokade!");
                }
            }
           

          
            // Posisjonen vår brikke lander på
            auto landingPos = draggedPieceOriginalNextPos;

            // Sjekk om vi tok noen brikker
            for (auto &player : players) {
                if (player.playernumber == current_player_index) continue;

                for (auto &piece : player.pieces) {
                    if (piece.home_start || piece.home_end) continue;

                    // Finn denne motspiller-brikkens posisjon
                    auto enemyPos = calculate_new_board_position(player.playernumber, piece, 0);

                    //om vi har landet på et sted der det står en annen brikke
                    if (enemyPos.first == landingPos.first && enemyPos.second == landingPos.second) {
                       
                        // Er det en fiendtlig brikke?
                        if (player.playernumber != current_player_index) {

                            // Fiendtlig rokade — kan ikke flytte dit
                            if (piece.rokade) {
                                invalidMove(players.at(current_player_index), players.at(current_player_index).pieces.at(dragging_piece_index % 4));
                                throw std::runtime_error("Du kan ikke lande på en rokade! DRIKK!");
                            }

                            // Sjekk om fiendtlig brikke står på sitt eget frifelt (startfelt)
                            const auto& enemyStartPos = BOARD_PATH.at(piece.start_index);
                            if (enemyPos.first == enemyStartPos.first && enemyPos.second == enemyStartPos.second) { 
                                // Brikken er beskyttet på sitt frifelt — ugyldig trekk
                                invalidMove(players.at(current_player_index), players.at(current_player_index).pieces.at(dragging_piece_index % 4));
                                throw std::runtime_error("Brikken står på sitt frifelt og kan ikke tas! DRIKK!");
                            }

                            // Vanlig fiendtlig brikke — ta den
                            invalidMove(player, piece);
                        }

                    }
                }
            }

                        
            
            //Gyldig drop! Flytt brikken og return
            flytt_brike(dragging_piece_index % 4, steps);

            updateAfterMove();
           return;
        }
    }


    //spiller flytter til et sted som ikke er i løpet av de neste 6 stepsene. også steder utenfor banen
    invalidMove(players.at(current_player_index), players.at(current_player_index).pieces.at(dragging_piece_index % 4));
    throw std::runtime_error("Du kan ikke flytte til det feltet!");

    
    dragging_piece_index = -1;

}

void LudolWindow::check_drag_n_drop() {
    Point mouse = get_mouse_coordinates();

    if (is_left_mouse_button_down()) {
        if (dragging_piece_index == -1) {
            // Sjekk om musen traff en brikke — start drag
            dragging_piece_index = find_piece_at(mouse.x, mouse.y);
        }
        if (dragging_piece_index != -1) {
            drag_x = mouse.x;
            drag_y = mouse.y;
        }
    } else {
        if (dragging_piece_index != -1) {
            // Sluppet — sjekk om det er en gyldig celle
            handle_drop(drag_x, drag_y);

           // info = "Du droppet: " + std::to_string(dragging_piece_index) + " letsgooo!!";

            dragging_piece_index = -1;
        }
    }
}

void LudolWindow::updateAfterMove() {
    

    // Oppdater rokade-status
    //setter først alle til false
    for (auto &player : players) {
        for (auto &piece : player.pieces) {
            piece.rokade = false;
        }
    }

    //går igjennom alle brikker og leter etter rokade
    for (auto &player : players) {
        for (auto &piece : player.pieces) {
            auto pos = calculate_screen_position(player.playernumber, piece);

            for (auto &other_player : players) {
                for (auto &other_piece : other_player.pieces) {
                    if (&piece == &other_piece) continue;

                    auto other_pos = calculate_screen_position(other_player.playernumber, other_piece);
                    if (pos == other_pos) {
                        piece.rokade = true;
                        other_piece.rokade = true;
                    }
                }
            }
        }
    }
}

// tegner opp selve ludo brettet
void LudolWindow::draw_board()
{

    // --- Hjørneområder (6x6 celler) ---
    draw_rectangle({BOARD_X, BOARD_Y}, CELL * 6, CELL * 6, COLORS.at(0));
    draw_rectangle({BOARD_X + CELL * 9, BOARD_Y}, CELL * 6, CELL * 6, COLORS.at(1));
    draw_rectangle({BOARD_X + CELL * 9, BOARD_Y + CELL * 9}, CELL * 6, CELL * 6, COLORS.at(2));
    draw_rectangle({BOARD_X, BOARD_Y + CELL * 9}, CELL * 6, CELL * 6, COLORS.at(3));

    // --- Hvite indre bokser ---
    draw_rectangle({BOARD_X + CELL, BOARD_Y + CELL}, CELL * 4, CELL * 4, TDT4102::Color::white);
    draw_rectangle({BOARD_X + CELL * 10, BOARD_Y + CELL}, CELL * 4, CELL * 4, TDT4102::Color::white);
    draw_rectangle({BOARD_X + CELL, BOARD_Y + CELL * 10}, CELL * 4, CELL * 4, TDT4102::Color::white);
    draw_rectangle({BOARD_X + CELL * 10, BOARD_Y + CELL * 10}, CELL * 4, CELL * 4, TDT4102::Color::white);

    // --- 2x2 startplasser med rutenett i hvert hjørne ---
    // Hver startplass er CELL*1.5 stor, med litt margin
    constexpr int PIECE = CELL * 3 / 2;
    constexpr int GAP = CELL / 4;

    for (int player = 0; player <= 3; player++) {
        const auto &playerStart = HOME_START.at(player);
        const auto &color = COLORS.at(player);
        for (int piece = 0; piece <= 3; ++piece) {
            const auto &piecePos = playerStart.at(piece);

            const float x = piecePos.first;
            const float y = piecePos.second;
            draw_rectangle({BOARD_X + static_cast<int>(std::round(((x-0.25) * CELL))), BOARD_Y+ static_cast<int>(std::round(((y-0.25) * CELL)))}, CELL*1.5, CELL*1.5, color);
        }
    }

    // --- Rutenett på banestripene ---
    // Horisontal stripe
    for (int row = 0; row <= 3; ++row)
        draw_line({BOARD_X, BOARD_Y + CELL * (6 + row)}, {BOARD_X + BOARD_SIZE, BOARD_Y + CELL * (6 + row)}, TDT4102::Color::black);
    for (int col = 0; col <= 15; ++col)
        draw_line({BOARD_X + col * CELL, BOARD_Y + CELL * 6}, {BOARD_X + col * CELL, BOARD_Y + CELL * 9}, TDT4102::Color::black);

    // Vertikal stripe
    for (int col = 0; col <= 3; ++col)
        draw_line({BOARD_X + CELL * (6 + col), BOARD_Y}, {BOARD_X + CELL * (6 + col), BOARD_Y + BOARD_SIZE}, TDT4102::Color::black);
    for (int row = 0; row <= 15; ++row)
        draw_line({BOARD_X + CELL * 6, BOARD_Y + row * CELL}, {BOARD_X + CELL * 9, BOARD_Y + row * CELL}, TDT4102::Color::black);

    // --- Fargede hjemløyper ---
    for (int col = 1; col < 6; ++col)
        draw_rectangle({BOARD_X + col * CELL, BOARD_Y + CELL * 7}, CELL, CELL, COLORS.at(0));
    for (int row = 1; row < 6; ++row)
        draw_rectangle({BOARD_X + CELL * 7, BOARD_Y + row * CELL}, CELL, CELL, COLORS.at(1));
    for (int col = 9; col < 14; ++col)
        draw_rectangle({BOARD_X + col * CELL, BOARD_Y + CELL * 7}, CELL, CELL, COLORS.at(2));
    for (int row = 9; row < 14; ++row)
        draw_rectangle({BOARD_X + CELL * 7, BOARD_Y + row * CELL}, CELL, CELL, COLORS.at(3));

    // --- Mitdrekanter (fisket rekkefølge) ---
    Point center = {BOARD_X + CELL * 7 + CELL / 2, BOARD_Y + CELL * 7 + CELL / 2};
    draw_triangle({BOARD_X + CELL * 6, BOARD_Y + CELL * 6}, {BOARD_X + CELL * 6, BOARD_Y + CELL * 9}, center, COLORS.at(0)); // venstre = gul
    draw_triangle({BOARD_X + CELL * 6, BOARD_Y + CELL * 6}, {BOARD_X + CELL * 9, BOARD_Y + CELL * 6}, center, COLORS.at(1));   // topp = blå
    draw_triangle({BOARD_X + CELL * 9, BOARD_Y + CELL * 6}, {BOARD_X + CELL * 9, BOARD_Y + CELL * 9}, center, COLORS.at(2));    // høyre = rød
    draw_triangle({BOARD_X + CELL * 6, BOARD_Y + CELL * 9}, {BOARD_X + CELL * 9, BOARD_Y + CELL * 9}, center, COLORS.at(3));  // bunn = grønn
}

void LudolWindow::draw_piece(const int x, const int y, const TDT4102::Color color, const bool isDraggedOriginal){
    draw_circle({x, y}, CELL*(isDraggedOriginal ? 1.1 : 0.95) / 2, TDT4102::Color::black);
    draw_circle({x, y}, CELL*0.85 / 2, color);
}

void LudolWindow::draw_piece(Piece piece, Player player)
{   
    //check if it is being dragged!
    const bool isDragged = ((player.playernumber * 4 + piece.piece_number) == dragging_piece_index);

    const std::pair<int, int> piecePos = calculate_screen_position(player.playernumber, piece); //henter ut posisjonen til brikken uten å flytte den

    draw_piece(piecePos.first, piecePos.second, player.color, isDragged);



    /*

    if (piece.home_start) {
        const std::pair<float, float> piecePos = HOME_START.at(player.playernumber).at(piece.piece_number);
   
        const float x = piecePos.first;
        const float y = piecePos.second;

        draw_piece(BOARD_X + static_cast<int>(std::round((x * CELL)+CELL/2.0)), BOARD_Y+ static_cast<int>(std::round((y * CELL)+CELL/2.0)), player.color, isDragged);
    }
    else if(piece.home_end){
        const int home_end_path_index = std::min(piece.steps_made - BOARD_PATH.size(), HOME_END.at(player.playernumber).size() - 1);

        const std::pair<int, int> piecePos = HOME_END.at(player.playernumber).at(home_end_path_index);

        draw_piece((BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2, player.color, isDragged);

    } else {
        const std::pair<int, int> piecePos = BOARD_PATH.at(piece.path_index);

        draw_piece((BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2, player.color, isDragged);
    }*/
}

void LudolWindow::draw_players(std::vector<Player> players)
{

    //går igjenom alle spillere og brikkene deres og tegner de
    for (const auto &player : players) {
        for (const auto &piece : player.pieces) {
            draw_piece(piece, player);

            //om det er rokade tegner de et to tall på brikken
            if (piece.rokade) {
                auto pos = calculate_screen_position(player.playernumber, piece);
                draw_text(
                    {pos.first - CELL / 4, pos.second - CELL / 4},
                    "2",
                    TDT4102::Color::white,
                    static_cast<unsigned int>(CELL / 2)
                );
            }
        }
    }
}

void LudolWindow::draw_infoText() {
    
    // Rød feilmelding fra throws (f.eks. "Du må flytte det antallet steg...")
    draw_text({PANEL_X, PANEL_Y}, move_error, TDT4102::Color::red, 18);

    // Rød melding fra invalidMove (f.eks. "Gul må drikke!")
    draw_text({PANEL_X, PANEL_Y + 25}, drink_message, TDT4102::Color::red, 18);

    // Sort skillelinje
    draw_line({PANEL_X, PANEL_Y + 50}, {PANEL_X + 500, PANEL_Y + 50}, TDT4102::Color::black);

    // Info-tekst (hvem sin tur / hva de kastet)
    draw_text({PANEL_X, PANEL_Y + 65}, info, TDT4102::Color::black, 20);
}

void LudolWindow::draw_poeng() {

    //y verdi vi starter å tegne
    int startY = PANEL_Y + 300;

    //går igjenom alle spillere og skriver statusen på de
    for (int i = 0; i < static_cast<int>(players.size()); ++i) {
        const auto& player = players[i];
        int rowY = startY + i * 40;

        // Spillernavn i spillerens farge
        draw_text({PANEL_X, rowY}, player.name, player.color, 20);

        // Stats i svart
        std::string stats = ": Antall drukket: " + std::to_string(player.antallDrukket)
                          + "  Antall spydd: " + std::to_string(player.antallSpydd);
        draw_text({PANEL_X + 80, rowY}, stats, TDT4102::Color::black, 20);
    }
}

void LudolWindow::roll_dice() {      

    
    //sjekker at det er gyldig tidspunkt for terning å bli trillet, om det er feil går vi til neste spiller
    if (state != GameWaitState::WaitingForRoll) {
        //Her kan vi ikke throwe, siden vi er i button callback!

        feil = "Du må flytte en brikke før du kan kaste terningen igjen!";
        current_player_index = (current_player_index + 1) % 4; // bytt spiller ved feil
        state = GameWaitState::WaitingForRoll; // gå tilbake til å vente på terningkast 
        dragging_piece_index = -1;

        return;
    }

    // generere et tilfeldig tall mellom 1 og 6
    dice_result = rand() % 6 + 1;
    std::cout << "Du kastet en " << dice_result << "!" << std::endl;

   // Hvis spilleren ikke har brikker ute og ikke kastet 1 eller 6, hopp over spiller
    if (!has_pieces_on_board(current_player_index) && dice_result != 1 && dice_result != 6) {
        tryNr++;

        //har personen noen forsøk igjen?
        if (tryNr < 3) {
            info = players.at(current_player_index).name + " kastet " + std::to_string(dice_result) + " og har ingen brikker på brettet. Kast igjen, du har " + std::to_string(3 - tryNr) + " forsøk igjen";
            skip_info_update = true;
            
            // State forblir WaitingForRoll — spilleren kan kaste igjen
            return;
        //spiller er tom for forsøk og vi går videre
        } else {
            info = players.at(current_player_index).name + " kastet " + std::to_string(dice_result) + " og har ingen forsøk igjen. Det er nå " + players.at((current_player_index + 1) % 4).name + " sin tur";
            tryNr = 0;
            current_player_index = (current_player_index + 1) % 4;
            state = GameWaitState::WaitingForRoll;
            skip_info_update = true;
            return;
        }
    }

    // Spilleren kan flytte — enten har brikker ute, eller kastet 1/6
    state = GameWaitState::WaiutingForMove;
    feil = "";
    dragging_piece_index = -1;
    move_error = "";
    drink_message = "";
    tryNr = 0; //antall ganger spilleren har kastet terningen på sin tur, nullstilles når noen får flytte
    skip_info_update = false; //når neste person kastes kan ifoen endre seg
    

    //Midlertidig:

   // flytt_brike(0, dice_result);

   // current_player_index = (current_player_index + 1) % 4;
}

Piece LudolWindow::flytt_brike_struct(Piece piece, const int steps_to_make){

   // for (int current_player_index = 0; current_player_index <= 3; ++current_player_index)
   // {
    const int playerpos_before = piece.path_index;

    //pathindex etter flytt. % (mod operator) fjerner alle hele runder rundt brette. 
    //om brikken har kommet til startposisjonen blir posisjonen satt til null igjne
    const int playerpos_after = (playerpos_before + steps_to_make) % BOARD_PATH.size();

    piece.home_start = false;
    piece.path_index = playerpos_after;
    piece.steps_made += steps_to_make;

    //Har vi gått rundt hele brettet?
    if (piece.steps_made >= BOARD_PATH.size()) {
        piece.home_end = true;
    }

    return piece;
}

/// @brief flytter antall steps og oppdaterer home_start og home_end
/// @param valgtBrikkeIndex 
/// @param steps_to_make 
void LudolWindow::flytt_brike(const int valgtBrikkeIndex, const int steps_to_make){

   

    Piece brikke = players.at(current_player_index).pieces.at(valgtBrikkeIndex);
    players.at(current_player_index).pieces.at(valgtBrikkeIndex) = flytt_brike_struct(brikke, steps_to_make);

    state = GameWaitState::WaitingForRoll;
    feil = "";
    dragging_piece_index = -1;
    

    // Kastet 6 (eller brukte 1 som en 6er) -> kast igjen
    if (steps_to_make == 6) {
        info = players.at(current_player_index).name + " kastet 6 og får kaste igjen!";
        skip_info_update = true;
        // Ikke bytt spiller
    } else {

        current_player_index = (current_player_index + 1) % 4;
    }
}

void LudolWindow::reset_game()
{
    Player gul("Gul", COLORS.at(0), 0);
    Player blaa("Blå", COLORS.at(1), 1);
    Player roed("Rød", COLORS.at(2), 2);
    Player gronn("Grønn", COLORS.at(3), 3);

    // vektor med alle spillerne
    players = {gul, blaa, roed, gronn};
    current_player_index = 0; // Index til hvilken spiller som har tur (0-3)
    dice_result = 0;
}

void LudolWindow::invalidMove(Player& personWhoFailed, Piece& deadPiece){
    personWhoFailed.antallDrukket ++;
    std::cout << "spiller faild og antaldrukket" << personWhoFailed.name << personWhoFailed.antallDrukket << std::endl;
    

    deadPiece.reset();


    std::string msg = personWhoFailed.name + " må drikke!";
    drink_message = msg;

}

//returnerer true om spilleren har noen brikker på brettet (ikke i hjemstart eller hjemmeend)
bool LudolWindow::has_pieces_on_board(int player_index) {
    for (const auto& piece : players.at(player_index).pieces) {
        if (!piece.home_start) {
            return true;
        }
    }
    return false;
}


void LudolWindow::player_spydde(int player_index) {
    if (player_index >= static_cast<int>(players.size())) return;

    players.at(player_index).antallSpydd++;

    /* if (players.at(player_index).antallSpydd >= 3) {
        std::string name = players.at(player_index).name;
        players.erase(players.begin() + player_index);
        
        info = name + " har spydd 3 ganger og er ute av spillet!";
        skip_info_update = true;

        // Juster current_player_index om nødvendig
        if (current_player_index >= static_cast<int>(players.size())) {
            current_player_index = 0;
        }
    } */
}

//tomme funskjoner

void LudolWindow::handle_click(int x, int y) {}
bool LudolWindow::check_winner() { 


    return false; }
void LudolWindow::write_result_to_file(const std::string &result) {}