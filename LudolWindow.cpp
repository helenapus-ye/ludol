#include "LudolWindow.h"
#include <iostream>
#include <functional>

/// kordinater for brettet, ligger visualisert i brett kordinater.png
// I toppen av LudolWindow.cpp
constexpr int BOARD_X = 20;
constexpr int BOARD_Y = 60;
constexpr int BOARD_SIZE = 600;
constexpr int CELL = BOARD_SIZE / 15;

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
    TDT4102::Color::pink,
    TDT4102::Color::blue,
    TDT4102::Color::red,
    TDT4102::Color::hot_pink,
};

const std::vector<std::vector<std::pair<float,float>>> HOME_START = {
    {{1.5,1.5},{3.5,1.5},{1.5,3.5},{3.5,3.5}},
    {{10.5, 1.5},{12.5,1.5},{10.5,3.5},{12.5,3.5}},
    {{10.5,10.5},{12.5,10.5},{10.5,12.5},{12.5,12.5}},
    {{1.5,10.5},{3.5,10.5},{1.5,12.5},{3.5,12.5}}
};

const std::vector<std::vector<std::pair<int, int>>> HOME_END {
    {{1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}},
    {{7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}},
    {{13, 7}, {12, 7}, {11, 7}, {10, 7}, {9, 7}},
    {{7, 13}, {7, 12}, {7, 11}, {7, 10}, {7, 9} }
};

LudolWindow::LudolWindow(int x, int y, int width, int height, const std::string &title)
    : AnimationWindow(x, y, width, height, title),
      reset_button({10, 10}, 100, 30, "Reset"),
      quit_button({120, 10}, 100, 30, "Quit"),
      dice_button({230, 10}, 100, 30, "Kast terning")
{
    add(reset_button);
    add(quit_button);
    add(dice_button);

    reset_button.setCallback(std::bind(&LudolWindow::reset_game, this));
    quit_button.setCallback(std::bind(&LudolWindow::close, this));
    dice_button.setCallback(std::bind(&LudolWindow::roll_dice, this));  
}

void LudolWindow::play()
{
    Player gul("Gul", COLORS.at(0), 0);
    Player blaa("Blå", COLORS.at(1), 1);
    Player roed("Rød", COLORS.at(2), 2);
    Player gronn("Grønn", COLORS.at(3), 3);

    if(!players.empty()) {
        throw std::runtime_error("Spillet er allerede i gang!");
    }
    // vektor med alle spillerne
    players = {gul, blaa, gronn, roed};

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
        draw_board();

        draw_players(players);

        // tilslutt
        next_frame();
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

void LudolWindow::draw_piece(Piece piece, Player player)
{   
    if (piece.home_start) {
        const std::pair<float, float> piecePos = HOME_START.at(player.playernumber).at(piece.piece_number);
   
        const float x = piecePos.first;
        const float y = piecePos.second;
        draw_circle({BOARD_X + static_cast<int>(std::round((x * CELL)+CELL/2.0)), BOARD_Y+ static_cast<int>(std::round((y * CELL)+CELL/2.0))}, CELL*0.95 / 2.0, TDT4102::Color::black);
        draw_circle({BOARD_X + static_cast<int>(std::round((x * CELL)+CELL/2.0)), BOARD_Y+ static_cast<int>(std::round((y * CELL)+CELL/2.0))}, CELL*0.85 / 2.0, player.color);
    }
    else if(piece.home_end){

        const std::pair<int, int> piecePos = HOME_END.at(player.playernumber).at(piece.steps_made - BOARD_PATH.size());

        draw_circle({(BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2}, CELL*0.95 / 2, TDT4102::Color::black);
        draw_circle({(BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2}, CELL*0.85 / 2, player.color);



    } else {
        const std::pair<int, int> piecePos = BOARD_PATH.at(piece.path_index);

        draw_circle({(BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2}, CELL*0.95 / 2, TDT4102::Color::black);
        draw_circle({(BOARD_X + piecePos.first * CELL)+CELL/2, (BOARD_Y + piecePos.second * CELL)+CELL/2}, CELL*0.85 / 2, player.color);
    }
}

void LudolWindow::draw_players(std::vector<Player> players)
{
    for (const auto &player : players)
    {
        for (const auto &piece : player.pieces)
        {
            draw_piece(piece, player);
        }
    }
}


void LudolWindow::roll_dice() {
      
    //index til player:
    //const int playerIndex = 0;

    const int brikkeIndex = 0;



    // generere et tilfeldig tall mellom 1 og 6
    const int dice_result = 1; //rand() % 6 + 1;
    std::cout << "Du kastet en " << dice_result << "!" << std::endl;
    
    //pathindex før flytt
   

   for (int playerIndex = 0; playerIndex <= 3; ++playerIndex)
   {
     const int playerpos_before = players.at(playerIndex).pieces.at(brikkeIndex).path_index;

    //pathindex etter flytt. % (mod operator) fjerner alle hele runder rundt brette. 
    //om brikken har kommet til startposisjonen blir posisjonen satt til null igjne
    const int playerpos_after = (playerpos_before + dice_result) % BOARD_PATH.size();


   players.at(playerIndex).pieces.at(brikkeIndex).home_start = false;

    players.at(playerIndex).pieces.at(brikkeIndex).path_index = playerpos_after;

    players.at(playerIndex).pieces.at(brikkeIndex).steps_made += dice_result;

    //Har vi gått rundt hele brettet?
    if (players.at(playerIndex).pieces.at(brikkeIndex).steps_made >= BOARD_PATH.size()) {
        players.at(playerIndex).pieces.at(brikkeIndex).home_end = true;
    }

   }

    
    
}

void LudolWindow::draw_poeng() {}
void LudolWindow::handle_click(int x, int y) {}
bool LudolWindow::check_winner() { return false; }
void LudolWindow::reset_game()
{

   /* try
    {
        std::cout << "Før vi kaller play\n";
         play(); //throws
         std::cout << "Etter at vi kalte play\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
   std::cout << "alt er bra reset game";*/


}
void LudolWindow::write_result_to_file(const std::string &result) {}
