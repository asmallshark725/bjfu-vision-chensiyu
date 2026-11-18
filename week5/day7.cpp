#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

#include "light.hpp"
#include "armor.hpp"

int main() {
    cv::Mat image = cv::imread("images/day6_blue.jpg", cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cout << "图片读取失败" << std::endl;
        return 1;
    }

    int minBrightness = 200;

    cv::Mat mask(image.rows, image.cols, CV_8UC1, cv::Scalar(0));

    // 先创建窗口，再创建窗口中的滑动条
    cv::namedWindow("Day7 Mask", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Min Brightness", "Day7 Mask", nullptr, 255);
    cv::setTrackbarPos("Min Brightness", "Day7 Mask", minBrightness);

    cv::imshow("Day7 Original", image);

    while (true) {
        minBrightness = cv::getTrackbarPos("Min Brightness", "Day7 Mask");
        // 每轮重新计算前，清除上一次的结果
        mask.setTo(0);

        for (int row = 0; row < image.rows; row++) {
            for (int col = 0; col < image.cols; col++) {
                cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);

                int blue = pixel[0];
                int green = pixel[1];
                int red = pixel[2];

                if (blue > minBrightness && green > minBrightness && red > minBrightness) {
                    mask.at<unsigned char>(row, col) = 255;
                }
            }
        }

        cv::imshow("Day7 Mask", mask);

        // 处理窗口事件，按 Esc 退出循环
        int key = cv::waitKey(30);
        if (key == 27) {
            break;
        }
    }

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::cout << "轮廓数量：" << contours.size() << std::endl;
    double minArea = 100;
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
        if (tilt > maxTilt) {
            continue;
        }
        if (ratio < 3.0) {
            continue;
        }

        Light light(rect.center.x, rect.center.y,
                    length, width, angle);

        lights.push_back(light);

        std::cout << "候选 " << i << "，偏离竖直：" << tilt
                  << " 度" << std::endl;

        for (int j = 0; j < 4; j++) {
            cv::line(result, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0), 2);
        }
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

    for (std::size_t i = 0; i < lights.size(); i++) {
        for (std::size_t j = i + 1; j < lights.size(); j++) {
            Armor armor(lights[i], lights[j]);
            if (!armor.isParallel(5.0)) {
                continue;
            }

            double xGap = std::abs(lights[i].getCenterX() - lights[j].getCenterX());
            double yGap = std::abs(lights[i].getCenterY() - lights[j].getCenterY());

            std::cout << "水平间距：" << xGap << "，竖直错位：" << yGap << std::endl;
            if (xGap <= 0) {
                continue;
            }

            double offsetRatio = yGap / xGap;
            std::cout << "相对错位：" << offsetRatio << std::endl;

            double maxOffsetRatio = 0.25;
            if (offsetRatio > maxOffsetRatio) {
                continue;
            }

            armors.push_back(armor);
            cv::Point center(cvRound(armor.getCenterX()), cvRound(armor.getCenterY()));

            cv::circle(result, center, 5, cv::Scalar(0, 0, 255), -1);

            std::cout << "灯条配对：" << i << "、" << j
                      << "，装甲板中心：(" << armor.getCenterX()
                      << ", " << armor.getCenterY() << ")"
                      << std::endl;
        }
    }

    cv::drawContours(result, filteredContours, -1, cv::Scalar(255, 0, 255), 2);
    cv::imshow("Day7 Contours", result);
    cv::imwrite("images/Day7_contours.png", result);
    cv::imshow("Day7 Contours", result);
    cv::waitKey(0);

    // 退出循环后，保存最后一次计算的掩膜
    bool saved = cv::imwrite("images/Day7_mask.png", mask);

    if (saved) {
        std::cout << "掩膜保存成功" << std::endl;
    } else {
        std::cout << "掩膜保存失败" << std::endl;
        return 1;
    }

    std::cout << "三通道最低值：" << minBrightness << std::endl;
    return 0;
}
