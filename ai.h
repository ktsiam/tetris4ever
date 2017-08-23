//Author: Kostas Tsiampouris, Aug-2017

#ifndef AI_H_
#define AI_H_

#include "board.h"
#include "evaluation.h"

using MOVE = usmall;

const uint WAIT_TIME = MICRO_IN_SEC*0.02; //time between frames
const small ROTAT    = 4;                 //number of rotations

//Moves deep the AI thinks (defined in main.cpp)
extern usmall DEPTH;


//Platform that allows automatic play (inheriting from Board)
class AI : private Board{
public:

        //MAIN FUNCTIONS
        AI();               //Constructor
        void auto_play();   //Machine-played tetris

private:
        
        //VARIABLE

        //holds the current position's evaluation
        Evaluation curr_eval;


        //HELPER FUNCTIONS

        //move selecting functions
        MOVE choose();                  //returns best move
        float search(MOVE, small depth);//recurses in all possibilities

        //move functions
        Change testMove(MOVE);   //makes a reversable move
        void cancelMove(Change); //cancels the move
        void make_move(MOVE);    //makes the final move & animation
                
        //miscellaneous
        Evaluation evaluate();   //returns evaluation of curr position
        void wait();             //sleeps for WAIT_TIME

};

#endif //AI_H_
