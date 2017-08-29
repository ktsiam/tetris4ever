//Author: Kostas Tsiampouris, Aug-2017

#ifndef AI_H_
#define AI_H_

#include "board.h"
#include "evaluation.h"

typedef uint8_t MOVE;
typedef float   EVAL_SUM;


const uint WAIT_TIME = MICRO_IN_SEC*0.02; //time between frames
const small ROTAT    = 4;                 //number of rotations

//Moves deep the AI thinks (defined in main.cpp)
extern small DEPTH;


//Platform that allows automatic play (inheriting from Board)
class AI : private Board{
public:

        //MAIN FUNCTIONS
        AI();                            //Constructor
        [[noreturn]] void auto_play();   //Machine-played tetris

private:

        //VARIABLE

        //holds the current position's evaluation
        Evaluation curr_eval;


        //HELPER FUNCTIONS

        //move selecting functions
        inline MOVE choose();                                  //returns best move
        inline EVAL_SUM search(const MOVE, const small depth); //recurses all possibilities
        inline bool redundant(const MOVE);                     //checks if move redundant

        //move functions
        inline Change testMove(const MOVE);   //makes a reversable move
        inline void cancelMove(const Change); //cancels the move
               void make_move(const MOVE);    //makes the final move & animation

        //miscellaneous
        inline Evaluation evaluate() const;   //returns evaluation of curr position
        inline void wait() const;             //sleeps for WAIT_TIME

};

#endif //AI_H_
