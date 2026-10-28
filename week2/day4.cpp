#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <iomanip>
int main(){
    auto score = 90;
    auto averge = 80.75;
    std::cout<<score<<std::endl;
    score = 95.5;
    std::cout<<score<<std::endl;
    std::cout<<averge<<std::endl;
    std::vector<int>scores;
    int n;
    std::cin>>n;
    std::cout<<n<<std::endl;
    for(int i=0;i<n;i++){
        int score;
        std::cin>>score;
        scores.push_back(score);
    }
    for( auto& value:scores){
        value += 5;
    }
    for(const auto value:scores){
        std::cout<<value<<" ";
    }
    std::cout<<std::endl;
    return 0;
}