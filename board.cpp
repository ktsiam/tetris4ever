//Author: Kostas Tsiampouris, Aug-2017

#include "board.h"
#include <unistd.h> //usleep()
#include <thread>   //std::thread
#include <cassert>  //assert()


//MAIN FUNCTIONS

Board::Board()
{
        locked = false;
        score = 0;

        //seeding random number generator
        srand(static_cast<uint>(time(NULL)));

        //declares board (1 layer more on each dimension)
        board = new bool*[X_MAX+1];
        for (small i = 0; i < X_MAX+1; ++i)
                board[i] = new bool[Y_MAX+1];

        //initializes board as empty
        for (small i = 0; i < X_MAX; ++i)
                for (small k = 0; k < Y_MAX; ++k)
                        board[i][k] = 0;

        //fills the extra layer (right and down) with blocks
        for (small i = 0; i < X_MAX; ++i)
                board[i][Y_MAX] = 1;

        for (small i = 0; i < Y_MAX; ++i)
                board[X_MAX][i] = 1;

        //initializing permutation of following pieces
        perm = new small[PIECE_NUM];
        for (small i = 0; i < PIECE_NUM; ++i)
                perm[i] = i;

        shufflePerm();

        //generates a new piece
        piece = new Piece(perm[0]);

        //filling reserve
        assert(RES_NUM < PIECE_NUM);

        reserve = new small[RES_NUM];
        for (small i = 0; i < RES_NUM; ++i)
                reserve[i] = perm[i+1];

        index = 1 + RES_NUM;
}

Board::~Board()
{
        for (small i = 0; i < X_MAX+1; ++i)
                delete []board[i];
        delete []board;
        delete []reserve;
        delete []perm;
        delete piece;
        system("/bin/stty cooked");
}

void Board::play()
{
        std::thread first(&Board::control, this);
        std::thread second(&Board::fall, this);
        first.join();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//SIMPLE MOVE FUNCTIONS

bool Board::down()
{
        locked = true;

        //checking if possible
        for (small i = 0; i < BOX_PER_PIECE; ++i)
                if (board[piece->box[i].x + piece->pieceCoord.x]
                    [piece->box[i].y + piece->pieceCoord.y + 1]){
                        locked = false;
                        return false;
                }

        ++piece->pieceCoord.y;

        locked = false;
        return true;
}

bool Board::right()
{
        locked = true;

        //checking if possible
        for (small i = 0; i < BOX_PER_PIECE; ++i)
                if (board[piece->box[i].x + piece->pieceCoord.x + 1]
                    [piece->box[i].y + piece->pieceCoord.y]){
                        locked = false;
                        return false;
                }

        ++piece->pieceCoord.x;

        locked = false;
        return true;
}

bool Board::left()
{
        locked = true;

        //checking if possible
        for (small i = 0; i < BOX_PER_PIECE; ++i)
                if (piece->box[i].x + piece->pieceCoord.x <= 0 ||
                    board[piece->box[i].x + piece->pieceCoord.x - 1]
                    [piece->box[i].y + piece->pieceCoord.y]){
                        locked = false;
                        return false;
                }

        --piece->pieceCoord.x;

        locked = false;
        return true;
}

bool Board::rotate()
{
        locked = true;

        //asserting it can rotate
        for (small i = 0; i < BOX_PER_PIECE; ++i){
                small x_next = piece->box[i].y + piece->pieceCoord.x;
                small y_next = piece->size - 1 - piece->box[i].x + piece->pieceCoord.y;
                if (x_next < 0 || board[x_next][y_next]){
                        locked = false;
                        return false;
                }
        }

        //rotating
        for (small i = 0; i < BOX_PER_PIECE; ++i){
                small temp_x = piece->box[i].x;
                piece->box[i].x = piece->box[i].y;
                piece->box[i].y = piece->size - 1 - temp_x;
        }

        locked = false;
        return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//COMPOSITE MOVE FUNCTIONS

bool Board::downPlus()
{
        locked = true;
        bool d = down();
        if (!d){
                append();
                if (!newPiece(true))
                        game_over();
        }
        locked = false;
        return d;
}

void Board::control()
{
        while(true){
                system ("/bin/stty cooked");
                print();
                system ("/bin/stty raw");
                char c;
                std::cin >> c;
                switch (c){
                        case 's': downPlus(); break;
                        case 'd': right(); break;
                        case 'a': left(); break;
                        case 'w': drop(); break;
                        case 'r': rotate(); break;
                        case 'Q': [[clang::fallthrough]];
                        case 'q': delete this;
                                exit(0);
                }
        }
}

void Board::fall()
{
        while(true)
        {
                while (locked){};
                downPlus();

                system ("/bin/stty cooked");
                print();
                system ("/bin/stty raw");
                usleep(MICRO_IN_SEC/FALL_SPEED);
        }
}

Change Board::append()
{
        locked = true;

        std::vector<Coord> sqAdded;
        //appending squares
        for (small i = 0; i < BOX_PER_PIECE; ++i){
                small sum_x = piece->box[i].x + piece->pieceCoord.x;
                small sum_y = piece->box[i].y + piece->pieceCoord.y;
                board[sum_x][sum_y] = 1;
                sqAdded.push_back(Coord(sum_x, sum_y));
        }

        //checks for full lines and clears them
        std::vector<small> lnCleared = check_lines();

        locked = false;
        return Change(lnCleared, sqAdded, piece->ID);
}

void Board::drop()
{               
        locked = true;

        int drop_count = Y_MAX;
        for (small i = 0; i < BOX_PER_PIECE; ++i) {
                int count = 0;
                while(!board[piece->box[i].x + piece->pieceCoord.x]
                      [piece->box[i].y + piece->pieceCoord.y + count + 1])
                        ++count;
                drop_count = std::min(drop_count, count);
        }
        
        piece->pieceCoord.y += drop_count;
        downPlus();

        locked = false;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//RESERVE PIECE FUNCTIONS

inline void Board::shufflePerm()
{
        //shuffling perm
        for (small i = PIECE_NUM; i > 0; --i){
                small r = rand()%i;
                std::swap(perm[i-1], perm[r]);
        }
}

inline small Board::nextPiece()
{
        if (index < PIECE_NUM-1)
                index++;
        else{
                shufflePerm();
                index = 0;
        }
        return perm[index];
}

bool Board::newPiece(const bool replace)
{
        locked = true;
        //renewing piece
        delete piece;
        piece = new Piece(reserve[0]);

        //if new piece does not fit return false (game ends)
        for (small i = 0; i < BOX_PER_PIECE; ++i)
                if (board[piece->box[i].x + INIT_X]
                    [piece->box[i].y + INIT_Y]){
                        for (small k = 0; k < RES_NUM-1; ++k)
                                reserve[k] = reserve[k+1];
                        return false;
                }

        //shifting reserve one piece
        for (small i = 0; i < RES_NUM-1; ++i)
                reserve[i] = reserve[i+1];

        //possibly replaces last piece in reserve
        if (replace)
                reserve[RES_NUM-1] = nextPiece();

        locked = false;
        return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//MISCELLANEOUS

void Board::game_over()
{
        locked = true;
        system ("/bin/stty cooked");
        print();

        //fills board with squres for ending animation
        for (small y = Y_MAX-1; y >= 0; --y)
                for (small x = 0; x < X_MAX; ++x){
                        board[x][y] = true;
                        print();
                        usleep(END_GAME_TIME);
                }

        std::cout << "\nYOU CLEARED " << score
             << " LINES!\nGOOD JOB!\n";
        delete this;
        exit(0);
}

//clears lines, returns which lines were cleared
inline std::vector<small> Board::check_lines()
{
        locked = true;

        //setting up vector
        std::vector<small> lnCleared;
        for (small y = 0; y < Y_MAX; ++y){
                small x(0);
                for (; x < X_MAX; ++x)
                        if (!board[x][y])
                                break;
                if (x == X_MAX)
                        lnCleared.push_back(y);
        }

        //killing lines and incrementing score;
        for (std::vector<small>::iterator it = lnCleared.begin();
             it != lnCleared.end(); ++it)
                kill_line(*it);

        locked = false;
        return lnCleared;
}

inline void Board::kill_line(const small n)
{
        for (small x = 0; x < X_MAX; ++x)
                for (small y = n; y > 0; --y)
                        board[x][y] = board[x][y-1];
        ++score;
}

void Board::print() const
{
        //adds an unecessarily large amount of \n to clear terminal window
        for (small i = 0; i < 51; ++i)
                std::cout << std::endl;

        //creating board to be printed
        char **printable = new char *[X_MAX];
        for (small x = 0; x < X_MAX; ++x)
                printable[x] = new char[Y_MAX];

        //filling it with board squares
        for (small x = 0; x < X_MAX; ++x)
                for (small y = 0; y < Y_MAX; ++y)
                        printable[x][y] = " x"[board[x][y]];

        //filling it with piece squares
        std::cout << "ID = " << (int)piece->ID << std::endl;
        for (small i = 0; i < BOX_PER_PIECE; ++i){
                printable[piece->pieceCoord.x + piece->box[i].x]
                        [piece->pieceCoord.y + piece->box[i].y] = 'o';
                std::cout << "(" << (int)(piece->pieceCoord.x + piece->box[i].x)
                          << ", " << (int)(piece->pieceCoord.y + piece->box[i].y) << ")";
        }
        std::cout << std::endl;

        //printing score
        std::cout << "\033[1;31mSCORE :: " << score << " :: !!!!\033[0m\n";
        for (small x = 0; x < X_MAX; ++x)
                std::cout << "__";
        std::cout << "___\n";

        //printing board
        for (small y = 0; y < Y_MAX; ++y){
                std::cout << "| ";

                for (small x = 0; x < X_MAX; ++x)
                        std::cout << printable[x][y] << " ";
                std::cout << "| ";

                //printing reserve pieces
                for (small x = 2; x >= 0; --x)
                        if (y / 5 >= RES_NUM)
                                break;
                        else if (y % 5 == 4)
                                std::cout << "--";
                        else{
                                assert(reserve[y/5] < PIECE_NUM);
                                assert(y/5 < RES_NUM);
                                std::cout << " o"[PRINTABLE_PIECE[reserve[y/5]][x][y%5]] << " ";
                        }

                std::cout << "\n";
        }
        std::cout << "|";
        for (small x = 0; x < X_MAX; ++x)
                std::cout << "II";
        std::cout << "I|\n";

        for (small x = 0; x < X_MAX; ++x)
                delete []printable[x];
                delete []printable;
}
