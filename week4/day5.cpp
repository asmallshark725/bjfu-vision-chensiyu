#include<iostream>
#include<vector>
#include<memory>
#include<string>
class Detector {
    public:
     void start() const{
        std::cout<<"检测器启动"<<std::endl;
     }
    virtual void detect() const = 0;
    virtual ~Detector() {
    std::cout << "Detector基类销毁" << std::endl;
}
};
class RedDetector : public Detector {
public:
    void detect() const override {
        std::cout << "执行红色目标检测" << std::endl;
    }
    ~RedDetector(){
        std::cout << "红色检测器销毁"<<std::endl;
    }
};
class BlueDetector : public Detector {
    public:
    void detect() const override {
        std::cout <<"执行蓝色目标检测"<<std::endl;
    };
    ~BlueDetector(){
        std::cout <<"蓝色检测器销毁"<<std::endl;
    }
};
void runDetector(const Detector& detector) {
    detector.start();
    detector.detect();
};
int main(){
    {
        std::unique_ptr<Detector> detector;
        std::string color;
        std::cout << "请输入颜色(red/blue)" << std::endl;
        std::cin>>color;
        if(color=="red"){
           detector=std::make_unique<RedDetector>();
        }else if(color=="blue"){
            detector=std::make_unique<BlueDetector>();
        }else{
            std::cout<<"颜色无效"<<std::endl;
            return 1;
        }
        detector->start();
        detector->detect();
        std::cout << "即将离开代码块" << std::endl;
    }
    std::cout << "main继续运行" << std::endl;
}