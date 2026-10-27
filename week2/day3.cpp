#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <iomanip>
int sumScores(const std::vector<int>& scores){
    int grade = 0;
    for(int score:scores){
        grade +=score;
    }
    return grade;
    //std::cout<<"grade:"<<grade<<std::endl;
}
int main(){
    int score,n;
    const int maxScore = 100;
    std::cout<<maxScore<<std::endl;
    int a = 90;
    //maxScore =a;
    const int& view =a;
    std::cout<<view<<std::endl;
    a = 80 ;
    std::cout<<view<<std::endl;
    //view=90;
    std::cin>>n;
    std::vector<int>scores;
    for(int i=0;i<n;i++){
        std::cin>>score;
        scores.push_back(score);
     }
     int g = sumScores(scores);
     std::cout<<g<<std::endl;
     return 0;
}