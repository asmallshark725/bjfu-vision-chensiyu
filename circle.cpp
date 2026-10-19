#include<iostream>
#include<iomanip>
int main(){
    double r;
    std::cin >> r;
    std::cout <<std::fixed<<std::setprecision(2) <<"面积="<< r*r*3.14159<<std::endl;
    std::cout <<"周长="<< r*2*3.14159<<std::endl;
    
    return 0;
}