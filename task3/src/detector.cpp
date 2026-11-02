#include "detector.hpp"
#include <iostream>
#include "armor.hpp"
void Detector::start() const{
        std::cout<<"检测器启动"<<std::endl;
     }

void matchArmors(){
    int x=0;
    int y=0;
    std::vector<Light> lights;
    lights.push_back(Light(100.0, 80.0, 20.0, 4.0, 5.0));
    lights.push_back(Light(160.0, 84.0, 20.0, 4.0, 7.0));
    lights.push_back(Light(300.0, 100.0, 20.0, 4.0, 18.0));
    lights.push_back(Light(360.0, 102.0, 20.0, 4.0, 20.0));
    lights.push_back(Light(500.0, 120.0, 20.0, 4.0, -15.0));
    lights.push_back(Light(560.0, 118.0, 20.0, 4.0, -12.0));
    for(int i=0;i<=5;i++){
        for(int j=i+1;j<=5;j++){
            x++;
            Armor armor(lights[i], lights[j]);
            if(armor.isParallel(5.0)){
                y++;
                std::cout<<i<<" "<<j<<" "<<"中心："<<armor.getCenterX()<<" "<<armor.getCenterY()<<std::endl;
            }
        }
    }
    std::cout<<"全部配对:"<<x<<std::endl;
    std::cout<<"候选数量:"<<y<<std::endl;
}
      Detector::~Detector() {
    std::cout << "Detector基类销毁" << std::endl;
}
    void RedDetector::detect() const  {
        std::cout << "执行红色目标检测" << std::endl;
        matchArmors();
    }
    RedDetector::~RedDetector(){
        std::cout << "红色检测器销毁"<<std::endl;
    }
       void BlueDetector::detect() const  {
        std::cout <<"执行蓝色目标检测"<<std::endl;
        matchArmors();
    };
    BlueDetector::~BlueDetector(){
        std::cout <<"蓝色检测器销毁"<<std::endl;
    }
    void runDetector(const Detector& detector) {
    detector.start();
    detector.detect();
}