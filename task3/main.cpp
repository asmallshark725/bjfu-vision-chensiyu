#include<iostream>
#include <cmath>
#include <vector>
#include <memory>
#include<string>
#include "light.hpp"
#include "armor.hpp"
#include "detector.hpp"
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
        runDetector(*detector);
        std::cout << "即将离开代码块" << std::endl;
    }
    std::cout << "main继续运行" << std::endl;
    return 0;
}