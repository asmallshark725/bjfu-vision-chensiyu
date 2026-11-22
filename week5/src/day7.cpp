#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

#include "light.hpp"
#include "armor.hpp"

int main() {
    cv::Mat image = cv::imread("data/day7_tests/02_multi.jpg", cv::IMREAD_COLOR);
    cv::Mat blueMask(image.rows, image.cols, CV_8UC1, cv::Scalar(0));
    cv::Mat redMask(image.rows, image.cols, CV_8UC1, cv::Scalar(0));

    int colorThreshold = 50;
    bool showColorMasks = false;

    if (image.empty()) {
        std::cout << "图片读取失败" << std::endl;
        return 1;
    }
    std::string targetColor;
    std::cout << "输入目标颜色(Red / Blue):";
    std::cin >> targetColor;
    if (targetColor != "Red" && targetColor != "Blue") {
        std::cout << "目标颜色只能是 Red 或 Blue" << std::endl;
        return 1;
    }
    int minBrightness = 200;

    cv::Mat mask(image.rows, image.cols, CV_8UC1, cv::Scalar(0));

    // 先创建窗口，再创建窗口中的滑动条
    cv::namedWindow("Day7 Mask", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Min Brightness", "Day7 Mask", nullptr, 255);
    cv::setTrackbarPos("Min Brightness", "Day7 Mask", minBrightness);
    cv::createTrackbar("Color Difference", "Day7 Mask", nullptr, 255);
    cv::setTrackbarPos("Color Difference", "Day7 Mask", colorThreshold);

    cv::imshow("Day7 Original", image);

    while (true) {
        minBrightness = cv::getTrackbarPos("Min Brightness", "Day7 Mask");
        colorThreshold = cv::getTrackbarPos("Color Difference", "Day7 Mask");

        // 每轮重新计算前，清除上一次的结果
        mask.setTo(0);
        blueMask.setTo(0);
        redMask.setTo(0);

        for (int row = 0; row < image.rows; row++) {
            for (int col = 0; col < image.cols; col++) {
                cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);

                int blue = pixel[0];
                int green = pixel[1];
                int red = pixel[2];

                if (blue > minBrightness && green > minBrightness && red > minBrightness) {
                    mask.at<unsigned char>(row, col) = 255;
                }

                if (blue - red > colorThreshold) {
                    blueMask.at<unsigned char>(row, col) = 255;
                }

                if (red - blue > colorThreshold) {
                    redMask.at<unsigned char>(row, col) = 255;
                }
            }
        }

        cv::imshow("Day7 Mask", mask);
        if (showColorMasks) {
            cv::imshow("Day7 Blue Color", blueMask);
            cv::imshow("Day7 Red Color", redMask);
        }

        // 处理窗口事件，按 Esc 退出循环
        int key = cv::waitKey(30);
        if (key == 27) {
            break;
        }
    }

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::cout << "轮廓数量：" << contours.size() << std::endl;

    double minArea = 50;
    std::vector<std::vector<cv::Point>> filteredContours;

    for (std::size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area < minArea) {
            continue;
        }

        filteredContours.push_back(contours[i]);
        std::cout << "轮廓：" << i << ",面积：" << area << std::endl;
    }

    cv::Mat result = image.clone();
    std::vector<Light> lights;
    std::vector<cv::RotatedRect> lightRects;

    for (std::size_t i = 0; i < filteredContours.size(); i++) {
        cv::RotatedRect rect = cv::minAreaRect(filteredContours[i]);

        double length = std::max(rect.size.width, rect.size.height);
        double width = std::min(rect.size.width, rect.size.height);
        if (width <= 0) {
            continue;
        }

        double ratio = length / width;

        std::cout << "候选 " << i
                  << "，宽：" << rect.size.width
                  << "，高：" << rect.size.height
                  << "，长宽比：" << ratio
                  << std::endl;

        cv::Point2f vertices[4];
        rect.points(vertices);

        cv::Point2f edge1 = vertices[1] - vertices[0];
        cv::Point2f edge2 = vertices[2] - vertices[1];
        cv::Point2f longEdge = edge1;
        if (cv::norm(edge2) > cv::norm(edge1)) {
            longEdge = edge2;
        }

        if (longEdge.y > 0) {
            longEdge.x = -longEdge.x;
            longEdge.y = -longEdge.y;
        }

        double angle = std::atan2(longEdge.x, -longEdge.y) * 180.0 / CV_PI;
        double tilt = std::atan2(std::abs(longEdge.x),
                                std::abs(longEdge.y)) * 180.0 / CV_PI;
        double maxTilt = 20.0;
        double minAspectRatio = 2.5;
        double maxAspectRatio = 12.0;

        if (tilt > maxTilt) {
            continue;
        }
        if (ratio < minAspectRatio || ratio > maxAspectRatio) {
            continue;
        }

        int padding = std::max(2, cvRound(width));
        cv::Rect colorRegion = rect.boundingRect();
        colorRegion.x -= padding;
        colorRegion.y -= padding;
        colorRegion.width += 2 * padding;
        colorRegion.height += 2 * padding;

        colorRegion = colorRegion & cv::Rect(0, 0, image.cols, image.rows);

        int blueCount = cv::countNonZero(blueMask(colorRegion));
        int redCount = cv::countNonZero(redMask(colorRegion));
        std::string colorName = "Unknown";
        int colorCount = blueCount + redCount;
        if (colorCount >= 20) {
            double blueShare = 1.0 * blueCount / colorCount;
            if (blueShare >= 0.8) {
                colorName = "Blue";
            } else if (blueShare <= 0.2) {
                colorName = "Red";
            }
        }

        std::cout << "候选 " << i
                  << "，蓝像素：" << blueCount
                  << "，红像素：" << redCount
                  << "，颜色：" << colorName << std::endl;

        if (colorName == "Unknown") {
            continue;
        }
        if (colorName != targetColor){
            continue;
        }

        Light light(rect.center.x, rect.center.y,
                    length, width, angle);
        light.setColor(colorName);

        lights.push_back(light);
        lightRects.push_back(rect);

        std::cout << "候选 " << i << "，偏离竖直：" << tilt
                  << " 度" << std::endl;

    }

    std::cout << "最终灯条候选数：" << lights.size() << std::endl;
    for (std::size_t i = 0; i < lights.size(); i++) {
        std::cout << "Light " << i
                  << "，中心：(" << lights[i].getCenterX()
                  << ", " << lights[i].getCenterY() << ")"
                  << "，长宽比：" << lights[i].aspectRatio()
                  << "，带方向角度：" << lights[i].getangle()
                  << std::endl;
    }

    std::vector<Armor> armors;
    std::vector<bool> pairedLights(lights.size(), false);

    for (std::size_t i = 0; i < lights.size(); i++) {
        for (std::size_t j = i + 1; j < lights.size(); j++) {
            if (lights[i].getColor() != lights[j].getColor()) {
                continue;
            }

            Armor armor(lights[i], lights[j]);
            if (!armor.isParallel(5.0)) {
                continue;
            }
            double shorterLength = std::min(lights[i].getLength(),lights[j].getLength());
            double longerLength = std::max(lights[i].getLength(),lights[j].getLength());
            double lengthSimilarity = shorterLength / longerLength;
            std::cout << "长度相似度：" << lengthSimilarity << std::endl;
            double minLengthSimilarity = 0.6;
            if(lengthSimilarity<minLengthSimilarity){
                continue;
            }
            double xGap = std::abs(lights[i].getCenterX() - lights[j].getCenterX());
            double yGap = std::abs(lights[i].getCenterY() - lights[j].getCenterY());

            std::cout << "水平间距：" << xGap
                      << "，竖直错位：" << yGap << std::endl;
            if (xGap <= 0) {
                continue;
            }

            double offsetRatio = yGap / xGap;
            std::cout << "相对错位：" << offsetRatio << std::endl;

            double maxOffsetRatio = 0.25;
            if (offsetRatio > maxOffsetRatio) {
                continue;
            }

            double meanLength = (lights[i].getLength() + lights[j].getLength()) / 2.0;
            if (meanLength < 0) {
                continue;
            }

            double spacingRatio = xGap / meanLength;
            std::cout << "间距与平均灯长之比：" << spacingRatio << std::endl;

            double minSpacingRatio = 1.0;
            double maxSpacingRatio = 4.0;

            if (spacingRatio < minSpacingRatio ||
                spacingRatio > maxSpacingRatio) {
                continue;
            }

            double pairScore = lengthSimilarity - offsetRatio;
            std::cout << "灯条对：" << i << "、" << j << "，配对评分：" << pairScore << std::endl;

            armors.push_back(armor);
            pairedLights[i] = true;
            pairedLights[j] = true;
            cv::Point center(cvRound(armor.getCenterX()), cvRound(armor.getCenterY()));

            cv::Scalar centerColor(0, 0, 255);
            if (lights[i].getColor() == "Blue") {
                centerColor = cv::Scalar(255, 0, 0);
            }
            cv::circle(result, center, 5, centerColor, -1);

            std::cout << "灯条配对：" << i << "、" << j
                      << "，装甲板中心：(" << armor.getCenterX()
                      << ", " << armor.getCenterY() << ")"
                      << std::endl;
        }
    }

    for (std::size_t i = 0; i < lights.size(); i++) {
        if (!pairedLights[i]) {
            continue;
        }

        cv::Scalar lightColor(0, 0, 255);
        if (lights[i].getColor() == "Blue") {
            lightColor = cv::Scalar(255, 0, 0);
        }

        cv::Point2f vertices[4];
        lightRects[i].points(vertices);
        for (int j = 0; j < 4; j++) {
            cv::line(result, vertices[j], vertices[(j + 1) % 4],
                     lightColor, 2);
        }

        cv::Rect bounds = lightRects[i].boundingRect();
        cv::Point labelPoint(std::max(0, bounds.x), std::max(12, bounds.y - 5));
        cv::putText(result, lights[i].getColor(), labelPoint,
                    cv::FONT_HERSHEY_SIMPLEX, 0.45, lightColor, 1, cv::LINE_AA);
    }

    std::cout << "最终装甲板候选数：" << armors.size() << std::endl;
    std::cout << "最终显示灯条数："
              << std::count(pairedLights.begin(), pairedLights.end(), true)
              << std::endl;
    cv::imshow("Day7 Contours", result);
    cv::imwrite("results/day7/Day7_contours.png", result);
    cv::waitKey(0);

    // 退出循环后，保存最后一次计算的掩膜
    bool saved = cv::imwrite("results/day7/Day7_mask.png", mask);

    if (saved) {
        std::cout << "掩膜保存成功" << std::endl;
    } else {
        std::cout << "掩膜保存失败" << std::endl;
        return 1;
    }

    std::cout << "三通道最低值：" << minBrightness << std::endl;
    std::cout << "色差门槛：" << colorThreshold << std::endl;
    return 0;
}
