#include <iostream>
#include <fstream>
#include <vector>
int main(){
    std::ifstream fin("scores.txt");
    
    if (!fin.is_open()) {
    std::cout << "无法打开 scores.txt" << std::endl;
    return 1;
}
    int n=0;
    if(!(fin >> n)||n<=0){
        std::cout<<"成绩数量无效"<<std::endl;
        return 1;
    }
    std::cout<<n<<std::endl;
    std::vector<int> scores;
    for(int i=0;i<n;i++){
        int score;
        if(!(fin>>score)){
            std::cout<<"成绩不足或格式错误"<<std::endl;
            return 1;
        }
        scores.push_back(score);
    }
    for(int score :scores){
        std::cout<<score<<" ";
    }
    std::cout<<std::endl;
    return 0;
}