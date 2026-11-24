#include "pnp_solver.hpp"

#include <cmath>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <stdexcept>

PnPSolver::PnPSolver(const std::string& cameraFile) {
    cv::FileStorage config(cameraFile, cv::FileStorage::READ);
    if (!config.isOpened()) throw std::runtime_error("Cannot open camera configuration: " + cameraFile);
    config["camera_matrix"] >> cameraMatrix_;
    config["distortion_coefficients"] >> distortion_;
    imageSize_ = {int(config["image_width"]), int(config["image_height"])};
    const double width = double(config["armor_width_m"]);
    const double height = double(config["light_length_m"]);
    if (cameraMatrix_.rows != 3 || cameraMatrix_.cols != 3 || distortion_.total() != 5 ||
        !std::isfinite(width) || !std::isfinite(height) || width <= 0 || height <= 0 ||
        imageSize_.width <= 0 || imageSize_.height <= 0)
        throw std::runtime_error("Invalid camera matrix, image size or armor dimensions.");
    cameraMatrix_.convertTo(cameraMatrix_, CV_64F);
    distortion_.convertTo(distortion_, CV_64F);
    if (!cv::checkRange(cameraMatrix_) || !cv::checkRange(distortion_) ||
        cameraMatrix_.at<double>(0, 0) <= 0 || cameraMatrix_.at<double>(1, 1) <= 0)
        throw std::runtime_error("Invalid camera parameters.");
    const float x = float(width / 2), y = float(height / 2);
    objectPoints_ = {{-x, -y, 0}, {x, -y, 0}, {x, y, 0}, {-x, y, 0}};
}

std::optional<PoseResult> PnPSolver::solve(const std::array<cv::Point2f, 4>& corners,
                                           const cv::Size& imageSize) const {
    if (imageSize != imageSize_)
        throw std::runtime_error("Video size differs from calibration. Supply matching camera parameters.");
    for (const auto& point : corners)
        if (!std::isfinite(point.x) || !std::isfinite(point.y)) return std::nullopt;
    const std::vector<cv::Point2f> points(corners.begin(), corners.end());
    if (!cv::isContourConvex(points) || std::abs(cv::contourArea(points)) < 4) return std::nullopt;

    cv::Mat rotationVector, translationVector;
    try {
        // 迭代法直接最小化 3D 物体点投影回图像后的误差。
        if (!cv::solvePnP(objectPoints_, points, cameraMatrix_, distortion_,
                          rotationVector, translationVector, false, cv::SOLVEPNP_ITERATIVE))
            return std::nullopt;
    } catch (const cv::Exception&) {
        return std::nullopt;  // 退化的角点几何不应中断整个视频。
    }
    if (!cv::checkRange(rotationVector) || !cv::checkRange(translationVector)) return std::nullopt;
    const cv::Vec3d rotation(rotationVector.at<double>(0), rotationVector.at<double>(1),
                             rotationVector.at<double>(2));
    const cv::Vec3d translation(translationVector.at<double>(0), translationVector.at<double>(1),
                                translationVector.at<double>(2));
    if (translation[2] <= 0) return std::nullopt;
    std::vector<cv::Point2f> projected;
    cv::projectPoints(objectPoints_, rotation, translation, cameraMatrix_, distortion_, projected);
    const double error = cv::norm(points, projected, cv::NORM_L2) / 2.0;
    if (!std::isfinite(error) || error > 5.0) return std::nullopt;
    return PoseResult{rotation, translation, cv::norm(translation), error};
}
