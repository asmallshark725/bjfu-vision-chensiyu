#include <iostream>
class Detector {
    public:
     void start() const{
        std::cout<<"检测器启动"<<std::endl;
     }
    virtual void detect() const = 0;
};
class RedDetector : public Detector {
public:
    void detect() const override {
        std::cout << "执行红色目标检测" << std::endl;
    }
};
class BlueDetector : public Detector {
    public:
    void detect() const override {
        std::cout <<"执行蓝色目标检测"<<std::endl;
    };
};
void runDetector(const Detector& detector) {
    detector.start();
    detector.detect();
};
int main(){
    RedDetector red;
    BlueDetector blue;
    runDetector(red);
    runDetector(blue);
    return 0;
}