#include <iostream>
#include <string>
int main(){
    int age = 19;
    double height = 1.75;
    char grade ='A';
    bool isMember = true;
    std::string name ="shark";
    
    std::cout <<"name:"<<name << std::endl;
    std::cout <<"age:"<<age << std::endl;
    std::cout <<"height:"<<height << std::endl;
    std::cout <<"grade:"<<grade << std::endl;
    std::cout <<"isMember:"<<std::boolalpha<<isMember << std::endl;
    
    return 0;
}