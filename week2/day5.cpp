#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <iomanip>
#include <string>
struct Robot{
    std::string name;
    int hp;
};
int main(){
    std::vector<Robot> robots{
    {"步兵", 200},
    {"英雄", 150},
    {"哨兵", 300}
};
    for(auto& robot:robots){
        if(robot.hp<200){
            robot.hp +=50;
        }
        std::cout<<robot.name<<" "<<robot.hp<<std::endl;
    }
    return 0;
}