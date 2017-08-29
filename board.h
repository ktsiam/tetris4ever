//Author: Kostas Tsiampouris, Aug-2017

#ifndef BOARD_H_
#define BOARD_H_

#include "piece.h"
#include <iostream> //std::cout, std::endl, uint (unsigned long)
#include <vector>   //std::vector

const  uint MICRO_IN_SEC = 1000000;         //microseconds in a sec (usleep)
const  uint END_GAME_TIME = MICRO_IN_SEC/50;//speed of ending animation
const  small RES_NUM = 6;                  //number of pieces in reserve

//board dimensions and speed (defined in main.cpp)
extern small Y_MAX;
extern small X_MAX;
extern uint  FALL_SPEED;


//Keeps Changes to board made by a move (allows reversing a move)
struct Change{

        //Constructors
               Change() = default;
        inline Change(const std::vector<small> a, const std::vector<Coord> b, const small id):
                linesCleared(a), boxesAdded(b), piece_id(id) {}

        std::vector<small> linesCleared; //vector of cleared lines
        std::vector<Coord> boxesAdded;   //vector of boxes added
        small piece_id;                  //id of used piece
};

//Platform in which the game is played
class Board{

public:

        //MAIN FUNCTIONS
        Board();      //Constructor
        ~Board();     //Destructor
        void play();  //User-played tetris

protected:

        //VARIABLES

        //board & piece
        bool **board;
        Piece *piece;

        //piece reserve and similar
        small *reserve, *perm, index;

        //score
        uint score;

        //movement lock for threads
        volatile bool locked;


        //HELPER FUNCTIONS

        //move functions (they return if possible)
        bool down();
        bool right();
        bool left();
        bool rotate();

        //composite move functions
        bool downPlus();              //also appends, removes lines, checks end_game
        [[noreturn]] void control();  //user controls movement
        [[noreturn]] void fall();     //causes piece to fall slowly
        Change append();              //appends piece, returns change in board
        void drop();                  //drops and appends piece

        //reserve-piece functions
        inline void shufflePerm();        //permutes piece-sack
        inline small nextPiece();         //generates new reserve piece
               bool newPiece(const bool); //replaces current piece, returns if game over
        //parameter tells if to refresh last piece in reserve

        //miscellaneous
        [[noreturn]] void game_over();             //end-game animation
               std::vector<small> check_lines();   //clears lines, returns which
        inline void kill_line(const small);        //kills a line (parameter)
               void print() const;                 //Prints board
};

#endif //BOARD_H_
