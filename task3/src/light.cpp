#include"light.hpp"
#include<iostream>
        double Light::aspectRatio() const {
            return length / width;
        }
        double Light::getCenterX()const {
            return centerX;
        }
        double Light::getCenterY()const {
            return centerY;
        }
        double Light::getangle()const {
            return angle;
        }
    void Light::setWidth(double value) {
        if(value<=0){
            std::cout<<"宽度必须大于0"<<std::endl;
            return;
        }else{width = value;}
        }
        Light::Light(double initialX, double initialY,double initialLength, double initialWidth ,double initialAngle) {
            centerX = initialX;
            centerY = initialY;
            length = initialLength;
            angle = initialAngle;
            setWidth(initialWidth);
        }
        Light::~Light(){
            
        }