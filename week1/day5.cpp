#include <iostream>

int main() {
    for (int i = 2; i <= 100; i++) {
        bool isPrime = true;

        for (int n = 2; n < i; n++) {
            if (i % n == 0) {
                isPrime = false;
                break;
            }
        }

        if (isPrime) {
            std::cout << i << " ";
        }
    }

    std::cout << std::endl;
    return 0;
}