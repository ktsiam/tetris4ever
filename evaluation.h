//Author: Kostas Tsiampouris, Aug-2017

#ifndef EVALUATION_H_
#define EVALUATION_H_

extern bool SHOW_EVAL;

//Stores evaluation factors of a Board arrangement
struct Evaluation
{
        //Constructor
        inline Evaluation(): sqCount(0), holes(0), avgHeight(0), variance(0) {}

        //evaluation parameters
        short sqCount;
        short holes;
        float avgHeight;
        float variance;

        //evaluation weights
        static const short sqC_idx = -7;
        static const short hole_idx = -7;
        static const short avgH_idx = 4;
        static const short var_idx = -10;

        //returns total evaluation
        inline float sum() const{
                return sqC_idx * sqCount + hole_idx * holes
                        + avgH_idx * avgHeight + var_idx * variance;
        }

        //prints detailed evaluation
        void print() const{
                if (!SHOW_EVAL) return;
                std::cout << "square count = " << (uint)sqCount << std::endl;
                std::cout << "holes = " << (uint)holes << std::endl;
                std::cout << "average height = " << Y_MAX - avgHeight << std::endl;
                std::cout << "variance = "
                          << variance << std::endl;
                          std::cout << "total = " << sum()/100 << std::endl;
        }
};

#endif //EVALUATION_H_
