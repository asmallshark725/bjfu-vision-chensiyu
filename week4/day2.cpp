#include <iostream>

class Light{
    private:
        double length = 20.0;
        double width = 4.0;
    
    public:
        double aspectRatio() const {
            return length / width;
        }
    void setWidth(double value) {
        if(value<=0){
            std::cout<<"宽度必须大于0"<<std::endl;
            return;
        }else{width = value;}
        }
        Light(double initialLength, double initialWidth) {
            length = initialLength;
            setWidth(initialWidth);
        }
        ~Light(){
            std::cout<<"灯条对象销毁"<<std::endl;
        }
};

int main() {
    Light first(30.0,5.0);
    std::cout << first.aspectRatio()<<std::endl;
    first.setWidth(0.0);
    std::cout << first.aspectRatio()<<std::endl;
    std::cout << "main中的操作结束"<<std::endl;
     {
        Light temp(20.0, 4.0);
        std::cout << "块内操作结束" << std::endl;
     }
    std::cout << "main继续运行" << std::endl;
    return 0;
}