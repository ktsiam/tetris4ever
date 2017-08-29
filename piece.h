//Author: Kostas Tsiampouris, Aug-2017

#ifndef PIECE_H_
#define PIECE_H_

#include <cstdint> //int8_t (small), uint8_t (usmall)

typedef int8_t small;

//stores coordinates
struct Coord{

        Coord() = default;
        inline Coord(small a, small b) : x(a), y(b) {}
        small x, y;
};

//useful constants (also for Board class)
const small PIECE_NUM = 7; //num of available pieces
const small BOX_PER_PIECE = 4;
const small PIECE_SIZE[PIECE_NUM] = { 3, 3, 3, 3, 3, 2, 4 }; //max(x,y) size
const small INIT_X = 4; //initial piece coordinates
const small INIT_Y = 0;
const Coord INIT_COORD(INIT_X, INIT_Y);
const Coord PIECE_LIB[PIECE_NUM][4] = //piece shape
{{Coord(2,0), Coord(0,1), Coord(1,1), Coord(2,1)},
 {Coord(0,0), Coord(1,0), Coord(2,0), Coord(2,1)},
 {Coord(1,0), Coord(2,0), Coord(0,1), Coord(1,1)},
 {Coord(1,0), Coord(0,1), Coord(1,1), Coord(1,2)},
 {Coord(0,0), Coord(1,0), Coord(1,1), Coord(2,1)},
 {Coord(0,0), Coord(1,0), Coord(0,1), Coord(1,1)},
 {Coord(0,1), Coord(1,1), Coord(2,1), Coord(3,1)}};


//stores block-piece attributes
struct Piece{

        //constructor
        inline Piece(small init){
                this->ID = init;
                this->size = PIECE_SIZE[ID];
                this->pieceCoord = INIT_COORD;

                for (small i = 0; i < BOX_PER_PIECE; ++i)
                        box[i] = PIECE_LIB[ID][i];
        }

        Coord box[BOX_PER_PIECE];  //coordinates of boxes
        Coord pieceCoord;          //position in board
        small size;                //max(x,y) size of piece
        small ID;                  //piece ID
};

//only used for printing at Board::print()
const bool PRINTABLE_PIECE[PIECE_NUM][4][4] = {{{0, 0, 1, 0},
                                                {1, 1, 1, 0},
                                                {0, 0, 0, 0},
                                                {0, 0, 0, 0}},

                                               {{1, 1, 1, 0},
                                                {0, 0, 1, 0},
                                                {0, 0, 0, 0},
                                                {0, 0, 0, 0}},

                                               {{0, 1, 1, 0},
                                                {1, 1, 0, 0},
                                                {0, 0, 0, 0},
                                                {0, 0, 0, 0}},

                                               {{0, 1, 0, 0},
                                                {1, 1, 0, 0},
                                                {0, 1, 0, 0},
                                                {0, 0, 0, 0}},

                                               {{1, 1, 0, 0},
                                                {0, 1, 1, 0},
                                                {0, 0, 0, 0},
                                                {0, 0, 0, 0}},

                                               {{1, 1, 0, 0},
                                                {1, 1, 0, 0},
                                                {0, 0, 0, 0},
                                                {0, 0, 0, 0}},

                                               {{0, 0, 0, 0},
                                                {1, 1, 1, 1},
                                                {0, 0, 0, 0},
                                                {0, 0, 0, 0}}};
#endif //PIECE_H_
