//Author: Kostas Tsiampouris, Aug-2017

#include "ai.h"
#include <cassert> //assert()

uint FALL_SPEED = 5;
small Y_MAX = 20; //height
small X_MAX = 10; //width
small DEPTH = 3;  //moves deep the AI thinks

void help();                 //prints help menu
bool setParams(int, char**); //sets parameters


//RUNS PROGRAM
int main(int argc, char *argv[])
{
        if (argc == 1){
                help();
                return 0;
        }

        if (setParams(argc, argv))
                (new Board())->play();
        else
                (new AI())->auto_play();
}

//prints instructions
void help()
{
        std::cout << std::string(20, '\n')
                  << "#############################################################\n"
                  << "#############################################################\n\n"
                  << "   PARAMETERS:\n"
                  << "   \t-a   : AI play                    default choice\n"
                  << "   \t-m   : manual play\n\n"
                  << "   \t-s=$ : manual play speed(1-15)    default = 3\n"
                  << "   \t-h=$ : board height(5-)           default = 20\n"
                  << "   \t-w=$ : board width(8-20)          default = 10\n"
                  << "   \t-d=$ : AI depth(1-4)              default = 3\n"
                  << "\n   MANUAL CONTROLS:\n   \tMOVE   : A-S-D\n   \tDROP   : W\n"
                  << "   \tROTATE : R\n\n   QUIT: Q\n"
                  << "\n##### By Kostas Tsiampouris, Tufts University, Aug-2017 #####\n"
                  << "#############################################################\n"
                  << std::string(5, '\n');
}

//sets paramaeters passed from command line and returns man-auto mode
bool setParams(int argc, char *argv[])
{
        bool manual(false);

        for (int i = 1; i < argc; ++i)
                switch(argv[i][1]){
                        case 'm' : manual = true;
                                break;
                        case 'a' : manual = false;
                                break;
                        case 'h' :
                                Y_MAX = atoi(&argv[i][3]);
                                assert(Y_MAX >= 5);
                                break;
                        case 'w' :
                                X_MAX = atoi(&argv[i][3]);
                                assert(8 <= X_MAX);
                                assert(X_MAX <= 20);
                                break;
                        case 's' :
                                FALL_SPEED = atoi(&argv[i][3]);
                                assert(0 < FALL_SPEED);
                                assert(FALL_SPEED <= 15);
                                break;
                        case 'd' :
                                DEPTH = atoi(&argv[i][3]);
                                assert(1 <= DEPTH);
                                assert(DEPTH <= 4);
                                break;
                }
        return manual;
}
