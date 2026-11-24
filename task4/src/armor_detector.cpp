#include "armor_detector.hpp"

#include <algorithm>
#include <cmath>
#include <opencv2/imgproc.hpp>
#include <stdexcept>
#include <utility>

ArmorDetector::ArmorDetector(std::string targetColor, DetectorParameters parameters)
    : targetColor_(std::move(targetColor)), parameters_(parameters) {
    if (targetColor_ != "Red" && targetColor_ != "Blue")
        throw std::invalid_argument("Target color must be Red or Blue.");
    setThresholds(parameters.brightness, parameters.colorDifference);
}

void ArmorDetector::setThresholds(int brightness, int colorDifference) {
    if (brightness < 0 || brightness > 255 || colorDifference < 0 || colorDifference > 255)
        throw std::invalid_argument("Thresholds must be between 0 and 255.");
    parameters_.brightness = brightness;
    parameters_.colorDifference = colorDifference;
}

std::vector<DetectedArmor> ArmorDetector::detect(const cv::Mat& image, cv::Mat* mask) const {
    if (image.empty() || image.type() != CV_8UC3)
        throw std::invalid_argument("Detector needs a non-empty BGR color image.");
    cv::Mat brightnessMask;
    const auto lights = findLights(image, brightnessMask);
    if (mask) *mask = brightnessMask;
    return matchLights(lights);
}

std::vector<LightCandidate> ArmorDetector::findLights(const cv::Mat& image, cv::Mat& mask) const {
    // 第一步：先找发白的亮芯，颜色随后只在候选灯条附近判断。
    const int threshold = parameters_.brightness;
    cv::inRange(image, cv::Scalar(threshold, threshold, threshold),
                cv::Scalar(255, 255, 255), mask);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_RECT, {3, 3}));
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<LightCandidate> lights;
    for (const auto& contour : contours) {
        if (cv::contourArea(contour) < parameters_.minArea) continue;
        const auto rect = cv::minAreaRect(contour);
        const double length = std::max(rect.size.width, rect.size.height);
        const double width = std::min(rect.size.width, rect.size.height);
        if (width <= 0 || length <= 0) continue;
        const double ratio = length / width;
        if (ratio < parameters_.minAspectRatio || ratio > parameters_.maxAspectRatio) continue;

        cv::Point2f vertices[4];
        rect.points(vertices);
        auto direction = vertices[1] - vertices[0];
        const auto otherEdge = vertices[2] - vertices[1];
        if (cv::norm(otherEdge) > cv::norm(direction)) direction = otherEdge;
        if (direction.y < 0) direction *= -1;  // 统一从灯条顶部指向底部。
        direction *= 1.0f / float(cv::norm(direction));
        const double angle = std::atan2(-direction.x, direction.y) * 180.0 / CV_PI;
        if (std::abs(angle) > parameters_.maxTilt) continue;

        const auto color = identifyColor(image, rect);
        if (color != targetColor_) continue;
        Light light(rect.center.x, rect.center.y, length, width, angle);
        light.setColor(color);
        lights.push_back({light, rect,
            rect.center - direction * float(length / 2.0),
            rect.center + direction * float(length / 2.0)});
    }
    std::sort(lights.begin(), lights.end(), [](const auto& a, const auto& b) {
        return a.light.getCenterX() < b.light.getCenterX();
    });
    return lights;
}

std::string ArmorDetector::identifyColor(const cv::Mat& image, const cv::RotatedRect& rect) const {
    // 第二步：沿灯条转动的窄区域，代替会吞入背景的宽大水平包围框。
    auto sample = rect;
    if (sample.size.width < sample.size.height) {
        sample.size.width *= 3.0f;
        sample.size.height *= 1.1f;
    } else {
        sample.size.height *= 3.0f;
        sample.size.width *= 1.1f;
    }
    const auto region = sample.boundingRect() & cv::Rect(0, 0, image.cols, image.rows);
    if (region.empty()) return "Unknown";
    cv::Point2f vertices[4];
    sample.points(vertices);
    std::vector<cv::Point> polygon;
    for (const auto& point : vertices) polygon.emplace_back(cvRound(point.x) - region.x,
                                                           cvRound(point.y) - region.y);
    cv::Mat localMask(region.size(), CV_8UC1, cv::Scalar(0));
    cv::fillConvexPoly(localMask, polygon, cv::Scalar(255));
    int redCount = 0, blueCount = 0;
    for (int y = 0; y < region.height; ++y) {
        const auto* pixels = image.ptr<cv::Vec3b>(y + region.y) + region.x;
        const auto* selected = localMask.ptr<unsigned char>(y);
        for (int x = 0; x < region.width; ++x) {
            if (!selected[x]) continue;
            const int blue = pixels[x][0], red = pixels[x][2];
            if (red >= parameters_.brightness && red - blue > parameters_.colorDifference) ++redCount;
            if (blue >= parameters_.brightness && blue - red > parameters_.colorDifference) ++blueCount;
        }
    }
    const int count = redCount + blueCount;
    if (count < 8) return "Unknown";
    const double redShare = double(redCount) / count;
    if (redShare >= parameters_.minColorShare) return "Red";
    if (redShare <= 1.0 - parameters_.minColorShare) return "Blue";
    return "Unknown";
}

std::vector<DetectedArmor> ArmorDetector::matchLights(const std::vector<LightCandidate>& lights) const {
    struct Pair { std::size_t left, right; double score; };
    std::vector<Pair> pairs;
    for (std::size_t i = 0; i < lights.size(); ++i) {
        for (std::size_t j = i + 1; j < lights.size(); ++j) {
            const auto& a = lights[i];
            const auto& b = lights[j];
            if (a.light.getColor() != b.light.getColor()) continue;
            const Armor armor(a.light, b.light);
            if (!armor.isParallel(parameters_.maxAngleDifference)) continue;
            const double similarity = std::min(a.light.getLength(), b.light.getLength()) /
                                       std::max(a.light.getLength(), b.light.getLength());
            if (similarity < parameters_.minLengthSimilarity) continue;
            const double meanLength = (a.light.getLength() + b.light.getLength()) / 2.0;
            if (meanLength <= 0) continue;

            // 第三步：在灯条自身方向上计算错位，装甲板倾斜时坐标轴一起转动。
            auto vertical = (a.bottom - a.top) / float(a.light.getLength()) +
                            (b.bottom - b.top) / float(b.light.getLength());
            const double norm = cv::norm(vertical);
            if (norm <= 0) continue;
            vertical *= float(1.0 / norm);
            const cv::Point2f horizontal(vertical.y, -vertical.x);
            const auto delta = b.rectangle.center - a.rectangle.center;
            const double offset = std::abs(delta.dot(vertical)) / meanLength;
            const double spacing = std::abs(delta.dot(horizontal)) / meanLength;
            if (offset > parameters_.maxOffsetRatio || spacing < parameters_.minSpacingRatio ||
                spacing > parameters_.maxSpacingRatio) continue;

            // 中间夹着另一根有效灯条时，避免跨越它组成大框。
            bool containsLight = false;
            for (std::size_t k = 0; k < lights.size(); ++k) {
                if (k == i || k == j) continue;
                const auto relative = lights[k].rectangle.center - a.rectangle.center;
                const double across = relative.dot(horizontal) / delta.dot(horizontal);
                if (across > 0.1 && across < 0.9 &&
                    std::abs(relative.dot(vertical)) < meanLength * 0.5) containsLight = true;
            }
            if (containsLight) continue;
            const double angleDifference = std::abs(a.light.getangle() - b.light.getangle());
            pairs.push_back({i, j, similarity - offset - angleDifference / 180.0});
        }
    }
    std::sort(pairs.begin(), pairs.end(), [](const auto& a, const auto& b) { return a.score > b.score; });
    std::vector<bool> used(lights.size(), false);
    std::vector<DetectedArmor> armors;
    for (const auto& pair : pairs) {
        if (used[pair.left] || used[pair.right]) continue;
        const auto* left = &lights[pair.left];
        const auto* right = &lights[pair.right];
        if (left->rectangle.center.x > right->rectangle.center.x) std::swap(left, right);
        armors.push_back({Armor(left->light, right->light),
                          {left->top, right->top, right->bottom, left->bottom}, pair.score});
        used[pair.left] = used[pair.right] = true;
    }
    return armors;
}
