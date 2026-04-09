#include "player.h"

//start indexene til de forskjellige spillerne
const int START_OFFSETS[4] = {0, 13, 26, 39}; 


//forteller hvordan er brikke blir lagd
Piece::Piece(int piece_number, int start_index)
    //path_index starter på start_index - 1 fordi når brikken kommer inn på brettet skal den være på index 0 i BOARD_PATH
    : piece_number(piece_number), path_index(start_index - 1), home_start(true),home_end(false), 
    oneround(false), steps_made(-1), rokade(false), start_index(start_index){
}

void Piece::reset() {
    path_index = start_index - 1;
    home_start = true;
    home_end = false;
    oneround = false;
    steps_made = -1;
    rokade = false;
}


// definerer at når vi lager en player setter navn, farge og playnr fra parametere
//antall drukket og spydd settes automatisk til 0
// det blir også automatisk lagd tre brikker med path_index -1
Player::Player(std::string name, TDT4102::Color color, int playernumber)
    : name(name), color(color), playernumber(playernumber),
      antallDrukket(0), antallSpydd(0), gameOver(false) {
    int start = START_OFFSETS[playernumber];
    for (int i = 0; i < 4; ++i) {
        pieces.push_back(Piece(i, start));
    }
}