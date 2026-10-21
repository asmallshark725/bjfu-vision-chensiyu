#include <iostream>

void sayHi() {                       // ← 定义：写下菜谱
    std::cout << "hi" << std::endl;
}

int main() {
    sayHi();                         // ← 调用：做一次
    sayHi();                         // ← 再做一次
    return 0;
}