#include <iostream>
#include <cmath>

class Light{
    private:
        double length = 20.0;
        double width = 4.0;
        double centerX = 0.0;
        double centerY = 0.0;
        double angle = 0.0;
    public:
        double aspectRatio() const {
            return length / width;
        }
        double getCenterX()const {
            return centerX;
        }
        double getCenterY()const {
            return centerY;
        }
        double getangle()const {
            return angle;
        }
    void setWidth(double value) {
        if(value<=0){
            std::cout<<"宽度必须大于0"<<std::endl;
            return;
        }else{width = value;}
        }
        Light(double initialX, double initialY,double initialLength, double initialWidth ,double initialAngle) {
            centerX = initialX;
            centerY = initialY;
            length = initialLength;
            angle = initialAngle;
            setWidth(initialWidth);
        }
        ~Light(){
            std::cout<<"灯条对象销毁"<<std::endl;
        }
};
class Armor{
    private:
    Light left;
    Light right;

    public:
    Armor(const Light& first,const Light& second): left(first),right(second){
    }
    double getCenterX() const{
        double X = (left.getCenterX()+right.getCenterX())/2;
        return X;
    }
    double getCenterY() const{
        double Y = (left.getCenterY()+right.getCenterY())/2;
        return Y;
    }
    bool isParallel(double maxAngleDiff) const {
    double M= std::abs(left.getangle() - right.getangle());// 计算角度差的绝对值
    return M <= maxAngleDiff; // 返回它是否 <= maxAngleDiff
    }
};
int main(){
    Light light(100.0,80.0,20.0,4.0,5.0);
    std::cout << light.getCenterX() << " "<< light.getCenterY() << std::endl;
    std::cout << light.aspectRatio() << std::endl;
    std::cout << light.getangle() <<std::endl;
    Light first(100.0, 80.0, 20.0, 4.0, 5.0);
    Light second(160.0, 84.0, 20.0, 4.0, 15.0);
    Armor armor(first, second);
    std::cout << armor.getCenterX()<<" "<<armor.getCenterY() <<std::endl;
    std::cout << armor.isParallel(5.0) << std::endl;
    return 0;
}