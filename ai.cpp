//By Konstantinos Tsiampouris 7/15/2017

#include "ai.h"
#include <unistd.h>  //usleep()
#include <cassert>   //assert()
#include <limits>    //std::numeric_limits
#include <thread>    //std::thread


//MAIN FUNCTIONS

AI::AI() : Board()
{
        assert(RES_NUM >= DEPTH-1);
        std::cout << "PLEASE WAIT...\n";
}

void AI::auto_play()
{
        bool *quit = new bool(false);

        //creating a thread to allow user to quit using 'Q'
        std::thread quitThread([](bool *quit){
                        char c;
                        while(1){
                                std::cin >> c;
                                if (c == 'q' || c == 'Q')
                                        *quit = true;
                        }
                }, quit);

        MOVE choice;
        while(!*quit){
                //choosing move
                choice = choose();
                usleep(WAIT_TIME);

                //making move
                make_move(choice);

                //allowing for direct keyboard input
                system("/bin/stty raw");
                system("/bin/stty cooked");
        }
        //delete this;
        exit(0);                
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

        //iterating through all possible moves
        for (MOVE m = 0; m < X_MAX*ROTAT; ++m){
                temp = search(m, DEPTH-1);
                if (temp > max){
                        max = temp;
                        optimal = m;
                }
        }
        //returning best
        return optimal;
}

inline EVAL_SUM AI::search(const MOVE move, const small depth)
{
        //checking if move is redundant
        if (redundant(move))
                return std::numeric_limits<EVAL_SUM>::lowest();

        Change Ch = testMove(move);
        EVAL_SUM max = -std::numeric_limits<EVAL_SUM>::infinity();
        EVAL_SUM temp;

        //if already calculated move flag, return bad evaluation
        if (Ch.piece_id == PIECE_NUM + 1)
                return std::numeric_limits<EVAL_SUM>::lowest();

        //if game_over flag, it returns bad evaluation
        if (Ch.piece_id == PIECE_NUM)
                return std::numeric_limits<EVAL_SUM>::lowest();

        //if max depth reached, it returns evaluation
        if (depth == 0){
                max = evaluate().sum();
                goto ret;
        }
        else
                //if max depth not reached yet, it recurses deeper
                for (MOVE m = 0; m < X_MAX*ROTAT; ++m){
                        temp = search(m, depth-1);
                        if (temp > max){
                                max = temp;
                        }
                }
ret:
        cancelMove(Ch);
        return max;
}

inline bool AI::redundant(MOVE move)
{
        return
                (piece->ID == 2 && (move%ROTAT == 2 || move%ROTAT == 3)) ||
                (piece->ID == 4 && (move%ROTAT == 2 || move%ROTAT == 3)) ||
                (piece->ID == 5 && move % ROTAT != 0)                    ||
                (piece->ID == 6 && (move%ROTAT == 2 || move%ROTAT == 3));

}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//MOVE FUNCTIONS

inline Change AI::testMove(const MOVE move)
{
        //to avoid calculating the same move
        bool covered = false;

        for (MOVE i = 0; i < move%4; ++i)
                rotate();

        while (left()){};

        for (MOVE i = 0; i < move/4; ++i)
                if (!right())
                        covered = true;

        while(down()){};

        //store change
        Change Ch = append();

        //if move already covered
        if (covered){
                newPiece(false);
                cancelMove(Ch);
                Ch.piece_id = PIECE_NUM+1; //flag that move covered
        }

        //replace piece, without refreshing last reserve
        else if (!newPiece(false)){
                cancelMove(Ch);
                Ch.piece_id = PIECE_NUM; //flag that game over
        }


        return Ch;
}

inline void AI::cancelMove(const Change ch)
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
                for (small x = 0; x < X_MAX; ++x){
                        for (small y = 0; y < *it; ++y)
                                board[x][y] = board[x][y+1];
                        board[x][*it] = 1;
                }
                --score;
        }

        //piece is unappended
        for (auto it = ch.boxesAdded.begin(); it != ch.boxesAdded.end(); ++it)
                board[it->x][it->y] = 0;
}

void AI::make_move(const MOVE move)
{
        curr_eval = evaluate();
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
        evaluate().print();
        wait();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//MISCELLANEOUS

inline Evaluation AI::evaluate() const
{
        Evaluation ev;

        //adding square count
        for (small x = 0; x < X_MAX; ++x)
                for (small y = 0; y < Y_MAX; ++y)
                        ev.sqCount += board[x][y];

        //holes
        bool *mask = new bool[X_MAX];
        for (small x = 0; x < X_MAX; ++x)
                mask[x] = false;

        for (small x = 0; x < X_MAX; ++x)
                for (small y = 0; y < Y_MAX; ++y)
                        if (!mask[x])
                                mask[x] = board[x][y];
                        else
                                ev.holes += !board[x][y];

        for (small x = 0; x < X_MAX; ++x)
                for (small y = 0; y < Y_MAX; ++y)
                        ev.avgHeight += y * board[x][y];
        //avg height
        ev.avgHeight /= ev.sqCount;

        //variance
        small *tops = new small[X_MAX];
        for (small x = 0; x < X_MAX; ++x){
                tops[x] = Y_MAX - 1;
                for (small y = 0; y < Y_MAX; ++y)
                        if (board[x][y]){
                                tops[x] = y;
                                break;
                        }
        }
        for (small x = 0; x < X_MAX; ++x){
                float var = tops[x] - ev.avgHeight;
                ev.variance += var*var;
        }
        ev.variance /= X_MAX;

        delete []mask;
        delete []tops;
        return ev;
}

inline void AI::wait() const
{
        usleep(WAIT_TIME);
}
