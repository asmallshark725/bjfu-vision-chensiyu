#pragma once

#include <array>
#include <opencv2/core.hpp>
#include <optional>
#include <string>
#include <vector>

struct PoseResult {
    cv::Vec3d rotation;
    cv::Vec3d translation;  // 相机坐标系：X 向右、Y 向下、Z 向前，单位米。
    double distance;       // 相机光心到装甲板中心的直线距离，单位米。
    double reprojectionError;
};

class PnPSolver {
public:
    explicit PnPSolver(const std::string& cameraFile);
    std::optional<PoseResult> solve(const std::array<cv::Point2f, 4>& corners,
                                     const cv::Size& imageSize) const;
    cv::Size imageSize() const { return imageSize_; }

private:
    cv::Mat cameraMatrix_;
    cv::Mat distortion_;
    cv::Size imageSize_;
    std::vector<cv::Point3f> objectPoints_;
};
