#include "light.hpp"
#include "armor.hpp"
#include<iostream>
    Armor::Armor(const Light& first,const Light& second): left(first),right(second){
    }
    double Armor::getCenterX() const{
        double X = (left.getCenterX()+right.getCenterX())/2;
        return X;
    }
    double Armor::getCenterY() const{
        double Y = (left.getCenterY()+right.getCenterY())/2;
        return Y;
    }
    bool Armor::isParallel(double maxAngleDiff) const {
    double M= std::abs(left.getangle() - right.getangle());// 计算角度差的绝对值
    return M <= maxAngleDiff; // 返回它是否 <= maxAngleDiff
    }