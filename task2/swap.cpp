#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <iomanip>
//传值只是将值传到定义的变量里，并不会修改原变量
//引用的话，引用的字符和原变量就是同一个东西，只是形式不同
//指针则包含了原变量的地址，对地址的原变量修改就会改变原变量
void swapValue(int a, int b) {
    int temp = a;
    a = b;
    b = temp;
    std::cout << "函数内：" << a << " " << b << std::endl;                
}
void swapPointer(int* a,int* b){
     int temp = *a;
     *a = *b;
     *b = temp;
}
void swapReference(int& a,int& b){
    int temp = a;
    a = b;
    b = temp;
}
int main(){
    int x = 3;
    int y = 7;
    swapValue(x,y);
    std::cout << "函数外：" << x << " " << y << std::endl;
    int* p = &x;
    *p = 10;
    std::cout << "指针修改后：" << x << std::endl;
    x = 3;
    y = 7;
    swapPointer(&x,&y);
    std::cout << "指针修改后：" << x <<","<< y << std::endl;
    x = 3;
    y = 7;
    swapReference(x, y);
     std::cout << "引用：" << x <<","<< y << std::endl;
    return 0;
}
