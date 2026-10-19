#include <iostream>
#include <iomanip>
int main(){
    double pi = 3.14159265;
    std::cout<<pi<<std::endl;
    std::cout<<std::fixed <<std::setprecision(2) <<pi <<std::endl;
    
    return 0;
} 