#include <iostream>
#include <cmath>
#include <vector>

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
    return 0;
}