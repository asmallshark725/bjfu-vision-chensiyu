#include "scores.hpp"

int Sumscore(const std::vector<int>& scores){
     int sum =0;
    for(int score:scores){
        sum += score;
    }
    return sum;
}