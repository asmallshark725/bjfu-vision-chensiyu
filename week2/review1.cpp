#include <iostream>
#include <iomanip>
#include <vector>
int main(){
    int n,score,max;
    std::cin>>n;
    std::vector<int>scores;
    for(int i=0;i<n;i++){
        std::cin>>score;
        scores.push_back(score);
    }
    for(int score:scores){
        std::cout<<score<<" ";
    }
    int sum =0;
    for(int score:scores){
        sum += score;
    }
    double ar = sum*1.0/n;
    for(int score:scores){
        if(max<=score){
            max=score;
        }
    }
    std::cout<<"sum:"<<sum<<std::endl;
    std::cout<<"ar:"<<std::fixed<<std::setprecision(2)<<ar<<std::endl;
    std::cout<<"max:"<<max<<std::endl;
    return 0;
}