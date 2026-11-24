#pragma once

#include "armor.hpp"
#include <array>
#include <opencv2/core.hpp>
#include <string>
#include <vector>

struct DetectorParameters {
    int brightness = 150;
    int colorDifference = 50;
    double minArea = 20.0;
    double minAspectRatio = 2.0;
    double maxAspectRatio = 15.0;
    double maxTilt = 45.0;
    double maxAngleDifference = 20.0;
    double minColorShare = 0.6;
    double minLengthSimilarity = 0.6;
    double maxOffsetRatio = 0.5;
    double minSpacingRatio = 0.8;
    double maxSpacingRatio = 4.0;
};

struct LightCandidate {
    Light light;                         // 继续使用任务三的 Light 类。
    cv::RotatedRect rectangle;
    cv::Point2f top;
    cv::Point2f bottom;
};

struct DetectedArmor {
    Armor armor;                         // 由两个任务三的 Light 构造。
    // 点序始终是：左上、右上、右下、左下，与 PnP 的物体点一一对应。
    std::array<cv::Point2f, 4> corners;
    double score;
};

class ArmorDetector {
public:
    explicit ArmorDetector(std::string targetColor, DetectorParameters parameters = {});
    std::vector<DetectedArmor> detect(const cv::Mat& image, cv::Mat* mask = nullptr) const;
    void setThresholds(int brightness, int colorDifference);
    std::vector<DetectedArmor> matchLights(const std::vector<LightCandidate>& lights) const;

private:
    std::vector<LightCandidate> findLights(const cv::Mat& image, cv::Mat& mask) const;
    std::string identifyColor(const cv::Mat& image, const cv::RotatedRect& rect) const;
    std::string targetColor_;
    DetectorParameters parameters_;
};
