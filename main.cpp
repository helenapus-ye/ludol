//
// This is example code from Chapter 2.2 "The classic first program" of
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
//
// This program outputs the message "Hello, World!" to the monitor

#include "std_lib_facilities.h"
#include "LudolWindow.h"


//------------------------------------------------------------------------------'

// C++ programs start by executing the function main
int main() {
    // Show a nice message in the Terminal window
    cout << "Hello, World!" << endl;
    LudolWindow game(100, 100,  1320, 800, "Ludol");
    
    game.play();

    return 0;
}

//------------------------------------------------------------------------------
