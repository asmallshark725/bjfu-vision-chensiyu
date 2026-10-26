#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <iomanip>
int main(){
    int n,score;
    std::vector<int>scores;
    std::cin>>n;
    if(n<=0){
        std::cout<<"无意义"<<std::endl;
        return 0;
    }
    for( int i=0; i<n; i++){
        std::cin>>score;
        scores.push_back(score);
    }
     int sum =0;
    for(int score:scores){
        sum += score;
    }
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