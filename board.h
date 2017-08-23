//Author: Kostas Tsiampouris, Aug-2017

#ifndef BOARD_H_
#define BOARD_H_

#include "piece.h"
#include <iostream> //std::cout, std::endl, uint (unsigned long)
#include <vector>   //std::vector

const  uint MICRO_IN_SEC = 1000000;         //microseconds in a sec (usleep)
const  uint END_GAME_TIME = MICRO_IN_SEC/50;//speed of ending animation
const  usmall RES_NUM = 6;                  //number of pieces in reserve

//board dimensions and speed (defined in main.cpp)
extern usmall Y_MAX;
extern usmall X_MAX;
extern usmall FALL_SPEED;


//Keeps Changes to board made by a move (allows reversing a move)
struct Change{

        //Constructors
        Change() = default;
        Change(std::vector<small> a, std::vector<Coord> b, small id):
        linesCleared(a), boxesAdded(b), piece_id(id) {};

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
        bool locked;


        //HELPER FUNCTIONS

        //move functions (they return if possible)
        bool down();  
        bool right();
        bool left();
        bool rotate();

        //composite move functions
        bool downPlus(); //also appends, removes lines, checks end_game
        void control();  //user controls movement
        Change append(); //appends piece, returns change in board
        void drop();     //drops and appends piece
        void fall();     //causes piece to fall slowly

        //reserve-piece functions
        void shufflePerm(); //permutes piece-sack
        small nextPiece();  //generates new reserve piece
        bool newPiece(bool);//replaces current piece, returns if game over
        //parameter tells if to refresh last piece in reserve

        //miscellaneous
        void game_over();                 //end-game animation        
        std::vector<small> check_lines(); //clears lines, returns which
        void kill_line(small);            //kills a line (parameter)
        void print(); //Prints board
};

#endif //BOARD_H_
