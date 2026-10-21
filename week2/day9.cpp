#include <iostream>
#include <vector>
#include <iomanip>
int main(){
    std::vector<int> scores;
    int n;
    std::cin>>n;
    if(n<=0){
        std::cout<<"进程结束"<<std::endl;
        return 0;
    }
    for(int i = 0;i<n;i++){
        int score;
        std::cin>>score;
        scores.push_back(score);
    }
    for(int i =0 ;i < scores.size();i++){
        std::cout<<scores[i]<<" ";
    }
    std::cout<<std::endl;
    int sum = 0;
     for(int score : scores){
        std::cout<< score <<" ";
        sum = sum + score;
     }
    std::cout<<std::endl;
    std::cout<<sum<<std::endl;
    double ar = sum*1.0/scores.size();
    std::cout<<std::fixed<<std::setprecision(2)<<ar<<std::endl;
    int maxScore = scores[0];
    for(int score:scores){
        if (score>maxScore){
            maxScore = score;
        }
    }
    std::cout<<"最高分："<<maxScore<<std::endl;
    int minScore = scores[0];
    for(int score:scores){
        if (score<minScore){
            minScore = score;
        }
    }
    std::cout<<"最低分:"<<minScore<<std::endl;
    return 0;
}
