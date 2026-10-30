#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <iomanip>
#include <fstream>
#include "scores.hpp"

int main(){
    int n,score;
    std::vector<int>scores;
    std::ifstream fin("scores.txt");
     if (!fin.is_open()) {
    std::cout << "无法打开 scores.txt" << std::endl;
    return 1;
}
    if(!(fin >> n)||n<=0){
        std::cout<<"成绩不足或格式错误"<<std::endl;
        return 1;
    }
    for( int i=0; i<n; i++){
         if(!(fin>>score)){
            std::cout<<"成绩不足或格式错误"<<std::endl;
            return 1;
    }
          scores.push_back(score);
}
    int sum = Sumscore(scores);
    double ar = sum*1.0/n;
    std::sort(scores.begin(),scores.end(),std::greater<int>());
    for(int score :scores){
        std::cout<<score<<" ";
    }
    std::cout<<std::endl;
    std::cout<<"max:"<<scores[0]<<std::endl;
    std::cout<<"min:"<<scores[scores.size()-1]<<std::endl;
    std::cout<<"sum:"<<sum<<std::endl;
    std::cout<<"ar:"<<std::fixed<<std::setprecision(2)<<ar<<std::endl;
    return 0;
}