#include<iostream>
#include<iomanip>
int maxOf3(int a,int b,int c){
    if(a>=b&&a>=c){
        return a;
    }
    if(b>=a&&b>=c){
        return b;
    }
    return c;
}
int main() {
    int a, b, c;
    std::cin >> a >> b >> c;

    int max = maxOf3(a, b, c);
    std::cout << max << std::endl;
    double average = (a+b+c)/3.0;
    std::cout<<std::fixed<<std::setprecision(2) << average << std::endl;
    return 0;
}