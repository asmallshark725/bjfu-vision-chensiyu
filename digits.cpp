#include <iostream>
#include <iomanip>
int main(){
    int a;
    std::cin>>a;
    int b,c,d;
    b = a/100;
    c = a/10%10;
    d = a%10;
    std::cout<<b+c+d<<std::endl;
    return 0;
}