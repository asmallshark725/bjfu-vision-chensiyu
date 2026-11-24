#include "armor_detector.hpp"
#include "pnp_solver.hpp"

#include <cmath>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <stdexcept>

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void fillRectangle(cv::Mat& image, const cv::RotatedRect& rectangle, cv::Scalar color) {
    cv::Point2f vertices[4];
    rectangle.points(vertices);
    std::vector<cv::Point> polygon;
    for (const auto& point : vertices) polygon.emplace_back(cvRound(point.x), cvRound(point.y));
    cv::fillConvexPoly(image, polygon, color);
}

int main(int argc, char** argv) {
    try {
        require(argc == 2, "Camera configuration argument is required.");
        ArmorDetector red("Red"), blue("Blue");
        // 两根灯条随装甲板整体旋转；在全局坐标中 yGap/xGap 已超过原来的门槛。
        const double angle = 35.0, radians = angle * CV_PI / 180;
        const cv::Point2f across(float(std::cos(radians)), float(std::sin(radians)));
        for (const auto color : {cv::Scalar(0, 0, 255), cv::Scalar(255, 0, 0)}) {
            cv::Mat image(400, 500, CV_8UC3, cv::Scalar::all(0));
            for (int sign : {-1, 1}) {
                const cv::Point2f center = cv::Point2f(250, 200) + across * float(sign * 75);
                fillRectangle(image, {center, {24, 90}, float(angle)}, color);
                fillRectangle(image, {center, {12, 80}, float(angle)}, cv::Scalar::all(255));
            }
            const bool isRed = color[2] > color[0];
            require((isRed ? red : blue).detect(image).size() == 1, "Rotated same-color armor should match.");
            require((isRed ? blue : red).detect(image).empty(), "Opposite color should be rejected.");
        }
        require(red.detect(cv::Mat(400, 500, CV_8UC3, cv::Scalar::all(0))).empty(), "Black image should have no detection.");
        cv::Mat uncolored(400, 500, CV_8UC3, cv::Scalar::all(0));
        fillRectangle(uncolored, {{150, 200}, {12, 80}, 0}, cv::Scalar::all(255));
        fillRectangle(uncolored, {{300, 200}, {12, 80}, 0}, cv::Scalar::all(255));
        require(red.detect(uncolored).empty(), "White-only bright bars must not become colored armors.");
        cv::Mat threeLights(400, 500, CV_8UC3, cv::Scalar::all(0));
        for (int x : {100, 200, 300}) {
            fillRectangle(threeLights, {{float(x), 200}, {24, 90}, 0}, {0, 0, 255});
            fillRectangle(threeLights, {{float(x), 200}, {12, 80}, 0}, cv::Scalar::all(255));
        }
        require(red.detect(threeLights).size() == 1, "A light may participate in at most one selected pair.");

        cv::FileStorage config(argv[1], cv::FileStorage::READ);
        cv::Mat camera, distortion;
        config["camera_matrix"] >> camera;
        config["distortion_coefficients"] >> distortion;
        const float x = float(double(config["armor_width_m"]) / 2);
        const float y = float(double(config["light_length_m"]) / 2);
        const std::vector<cv::Point3f> object{{-x, -y, 0}, {x, -y, 0}, {x, y, 0}, {-x, y, 0}};
        const cv::Vec3d expected(0.08, -0.03, 1.6);
        std::vector<cv::Point2f> projected;
        cv::projectPoints(object, cv::Vec3d(0.22, 0.32, -0.18), expected, camera, distortion, projected);
        std::array<cv::Point2f, 4> points{projected[0], projected[1], projected[2], projected[3]};
        PnPSolver solver(argv[1]);
        const auto pose = solver.solve(points, solver.imageSize());
        require(pose.has_value(), "Projected planar points should solve.");
        require(cv::norm(pose->translation - expected) < 1e-4, "PnP must recover known translation in meters.");
        require(std::abs(pose->distance - cv::norm(expected)) < 1e-4, "Distance must be norm(tvec), not just Z.");
        std::swap(points[1], points[2]);
        require(!solver.solve(points, solver.imageSize()), "Self-crossing point order must be rejected.");
        bool rejectedSize = false;
        try { solver.solve(points, {640, 512}); } catch (const std::runtime_error&) { rejectedSize = true; }
        require(rejectedSize, "Mismatched image calibration must be rejected.");
        std::cout << "PASS: rotated red/blue pairing, uncolored rejection, one-to-one pairing, known PnP pose, point order and calibration size.\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
