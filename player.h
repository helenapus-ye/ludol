#pragma once
#include <vector>
#include <string>
#include "AnimationWindow.h"

/// informasjonen om en spillbrikke
struct Piece {
    int piece_number;
    int start_index; //indexen som path index alltid starter på, skal aldri endres
    int path_index; // index i BOARD_PATH for å finne brikkens posisjon
    int steps_made; // antall flytt fra sitt startpunkt i BOARD_PATH
    bool home_start;      // er brikken på startfeltet?
    bool home_end;        // er brikken på den pilen banen inn til mål?
    bool oneround;  // har brikken fullført en runde rundt brettet?
    bool rokade; //står brikken i en rokade

    //setter tilbakke alle orginal verdien, som når brikke blir tatt
    void reset(); //eks: players.at(player_index).pieces.at(piece_index).reset();

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