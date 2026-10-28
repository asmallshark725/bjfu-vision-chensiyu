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
void healrobot(Robot& name,int cure){
     name.hp +=cure;
}
void printrobot(const Robot& robot){
     std::cout<<robot.name<<" "<<robot.hp<<std::endl;
}
int main(){
    std::vector<Robot>robots{
        {"英雄",150},
        {"哨兵",200}
    };
    printrobot(robots[0]);
    printrobot(robots[1]);
    healrobot(robots[0],50);
    healrobot(robots[1],100);
    printrobot(robots[0]);
    printrobot(robots[1]);
    return 0;
}