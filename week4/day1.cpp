#include <iostream>

class Light {
public:
    double length = 20.0;
    double width = 4.0;
    double aspectRatio() {
    return length / width;
}
};

int main() {
    Light first,second;
    first.length = 30.0;
    second.width = 5.0;
    std::cout << first.length << std::endl;
    std::cout << second.length << std::endl;
    std::cout << first.aspectRatio()<<" "<<second.aspectRatio()<<std::endl;
    return 0;
}