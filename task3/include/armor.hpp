#pragma once
#include "light.hpp"
#include <cmath>
class Armor{
    private:
    Light left;
    Light right;

    public:
    Armor(const Light& first,const Light& second);
    double getCenterX() const;
    double getCenterY() const;
    bool isParallel(double maxAngleDiff) const ;
};