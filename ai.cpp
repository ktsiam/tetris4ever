//By Konstantinos Tsiampouris 7/15/2017

#include "ai.h"
#include <unistd.h>  //usleep()
#include <cassert>   //assert()
#include <limits>    //std::numeric_limits
#include <thread>    //std::thread

using EVAL_SUM = float;


//MAIN FUNCTIONS

AI::AI() : Board()
{
        assert(RES_NUM >= DEPTH-1);
        this->curr_eval = evaluate();
}

void AI::auto_play()
{
        //creating a thread to allow user to quit using 'Q'
        std::thread quitThread([](){
                        char c; 
                        while(1){
                                std::cin >> c;
                                if(c == 'q'){
                                        system("/bin/stty cooked");
                                        exit(0);
                                }
                        }
                });

        MOVE choice;
        while(true){
                //choosing move
                choice = choose();
                usleep(WAIT_TIME);
                curr_eval = evaluate();
                
                //applying it
                make_move(choice);

                //allowing for direct keyboard input
                system("/bin/stty raw");
                system("/bin/stty cooked");
        }
        quitThread.join();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//MOVE SELECTING FUNCTIONS

MOVE AI::choose()
{        
        EVAL_SUM max = -std::numeric_limits<float>::infinity();
        EVAL_SUM temp;
        MOVE optimal;

        //recurses through all possible future sequences
        for (MOVE m = 0; m < (X_MAX-1)*ROTAT; ++m){
                temp = search(m, DEPTH-1);
                if (temp > max){
                        max = temp;
                        optimal = m;
                }
        }
        //returns best first move
        return optimal;
}

EVAL_SUM AI::search(MOVE move, small depth)
{
        Change Ch = testMove(move);
        EVAL_SUM max = -std::numeric_limits<EVAL_SUM>::infinity();
        EVAL_SUM temp;
        MOVE optimal;

        //if game_over flag, return bad evaluation
        if (Ch.piece_id == PIECE_NUM)
                return std::numeric_limits<EVAL_SUM>::lowest();        
        
        //if max depth reached, return evaluation of position
        if (depth == 0){
                max = evaluate().sum();
                goto ret;
        }
        else
                //if max depth not reached yet, recurse deeper
                for (MOVE m = 0; m < (X_MAX-1)*ROTAT; ++m){
                        temp = search(m, depth-1);        
                        if (temp > max){
                                max = temp;
                                optimal = m;                        
                        }
                }
        
ret:    
        cancelMove(Ch);
        return max;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//MOVE FUNCTIONS

Change AI::testMove(MOVE move)
{
        for (MOVE i = 0; i < move%4; ++i)
                rotate();                       
        
        while (left()){};
        
        for (MOVE i = 0; i < move/4; ++i)
                right();

        while(down()){};
        
        //store change
        Change Ch = append();

        //replace piece, without refreshing last reserve
        if (!newPiece(false)){
                cancelMove(Ch);
                Ch.piece_id = PIECE_NUM; //flag that game over
        }
        return Ch;
}

void AI::cancelMove(Change ch)
{
        //reserve is put back to normal
        for (small i = RES_NUM-1; i >= 1; --i)
                reserve[i] = reserve[i-1];
        reserve[0] = piece->ID;
        
        //piece is replaced with previous one
        delete piece;
        piece = new Piece(ch.piece_id);
        
        //lines are added back        
        for (auto it = ch.linesCleared.rbegin(); it != ch.linesCleared.rend(); ++it){
                for (usmall x = 0; x < X_MAX; ++x){
                        for (usmall y = 0; y < *it; ++y)
                                board[x][y] = board[x][y+1];
                        board[x][*it] = 1;
                }
                --score;
        }
        
        //piece is unappended
        for (auto it = ch.boxesAdded.begin(); it != ch.boxesAdded.end(); ++it)
                board[it->x][it->y] = 0;
}

void AI::make_move(MOVE move)
{
        for (MOVE i = 0; i < move%4; ++i){
                rotate();
                print();
                curr_eval.print();
                wait();
        }
        
        while (left()){
                print();
                curr_eval.print();
                wait();
        }
        
        for (MOVE i = 0; i < move/4; ++i)
                if (right()){
                        print();
                        curr_eval.print();
                        wait();
                }
        drop();
        print();
        curr_eval.print();
        wait();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//MISCELLANEOUS

Evaluation AI::evaluate()
{
        Evaluation ev;

        //adding square count
        for (usmall x = 0; x < X_MAX; ++x)
                for (usmall y = 0; y < Y_MAX; ++y)
                        ev.sqCount += board[x][y];
        
        //holes
        bool *mask = new bool[X_MAX];
        for (usmall x = 0; x < X_MAX; ++x)
                mask[x] = false;
        
        for (usmall x = 0; x < X_MAX; ++x)
                for (usmall y = 0; y < Y_MAX; ++y)
                        if (!mask[x])
                                mask[x] = board[x][y];
                        else
                                ev.holes += (!board[x][y]);
        
        for (usmall x = 0; x < X_MAX; ++x)
                for (usmall y = 0; y < Y_MAX; ++y)
                        ev.avgHeight += y * board[x][y];
        //avg height
        ev.avgHeight /= ev.sqCount;
        
        //creating mask
        small *tops = new small[X_MAX];        
        for (usmall x = 0; x < X_MAX; ++x){
                tops[x] = Y_MAX - 1;
                for (usmall y = 0; y < Y_MAX; ++y)
                        if (board[x][y]){
                                tops[x] = y;
                                break;
                        }
        }
        for (usmall x = 0; x < X_MAX; ++x){
                float var = tops[x] - ev.avgHeight;
                ev.variance += var*var;
        }
        ev.variance /= X_MAX;

        delete []mask;
        delete []tops;
        return ev;
}

void AI::wait()
{
        usleep(WAIT_TIME);
}
