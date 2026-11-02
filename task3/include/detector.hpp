#pragma once
#include<iostream>
#include<vector>
#include<memory>
#include<string>
class Detector {
    public:
     void start() const;
    virtual void detect() const = 0;
    virtual ~Detector();
};
void matchArmors();
class RedDetector : public Detector {
public:
    void detect() const override;
    ~RedDetector();
};
class BlueDetector : public Detector {
    public:
    void detect() const override ;
    ~BlueDetector();
};
void runDetector(const Detector& detector) ;