#pragma once
#include <vector>
#include <string>

class Light{
    private:
        double length = 20.0;
        double width = 4.0;
        double centerX = 0.0;
        double centerY = 0.0;
        double angle = 0.0;
        std::string color = "Unknown";
    public:
        double getLength() const;
        double aspectRatio() const ;
        double getCenterX()const ;
        double getCenterY()const ;
        double getangle()const ;
        std::string getColor() const;
        void setColor(const std::string& value);
    void setWidth(double value) ;
        Light(double initialX, double initialY,double initialLength, double initialWidth ,double initialAngle) ;
        ~Light();
};