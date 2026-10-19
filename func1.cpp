#include <iostream>
double area(double r, double pi = 3.14159) {
    return pi * r * r;
}
int main() {
    std::cout << area(2.0) << std::endl;
    std::cout << area(2.0, 3.0) << std::endl;
    return 0;
}