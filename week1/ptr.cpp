#include <iostream>

int main() {
    int a = 42;
    int* p = &a;

    std::cout << "a  = " << a  << std::endl;
    std::cout << "&a = " << &a << std::endl;
    std::cout << "p  = " << p  << std::endl;
    std::cout << "*p = " << *p << std::endl;

    *p = 100;

    std::cout << "after *p = 100, a = " << a << std::endl;

    return 0;
}