#pragma once
#include <vector>
#include <string>
#include "AnimationWindow.h"

/// informasjonen om en spillbrikke
struct Piece {
    int piece_number;
    int path_index; // index i BOARD_PATH for å finne brikkens posisjon
    bool home;      // er brikken på startfeltet?
    bool oneround;  // har brikken fullført en runde rundt brettet?
    
    Piece(int piece_number, int start_index);
};


//informasjonen om hver spiller
struct Player {
    //navn på spiller
    std::string name;

    //player NR, fra topp venstre og med klokka
    int playernumber;

    //farge på spiller
    TDT4102::Color color;

    //brikkene til spilleren
    std::vector<Piece> pieces; // 4 brikker per spiller

    //antall drukket
    int antallDrukket;

    //antall spyd
    int antallSpydd;



    Player(std::string name, TDT4102::Color color, int playernumber);
};