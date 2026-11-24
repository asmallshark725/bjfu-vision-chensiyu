#include "light.hpp"
#include "armor.hpp"
#include<iostream>
#include<opencv2/opencv.hpp>
int main() {
    cv::VideoCapture cap("/mnt/e/robomaster/school_training/hello_armor/video.avi");
    if (!cap.isOpened()) {
        std::cout<<"视频打开失败"<<std::endl;
        return 1;
    }
    cv::Mat frame;
    //----------创建掩膜滑块，指定检测颜色----------
    int colorThreshold = 50;
    bool showColorMasks = false;
    std::string targetColor;
    std::cout << "输入目标颜色(Red / Blue):";
    std::cin >> targetColor;
    if (targetColor != "Red" && targetColor != "Blue") {
        std::cout << "目标颜色只能是 Red 或 Blue" << std::endl;
        return 1;
    }
    int minBrightness = 200;
    cv::namedWindow("Video Mask", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Min Brightness", "Video Mask", nullptr, 255);
    cv::setTrackbarPos("Min Brightness", "Video Mask", minBrightness);
    cv::createTrackbar("Color Difference", "Video Mask", nullptr, 255);
    cv::setTrackbarPos("Color Difference", "Video Mask", colorThreshold);
    //------开始循环播放-------
    while(true){
        if(!cap.read(frame)){
            std::cout<<"视频结束或读取失败"<<std::endl;
            break;
        }
        minBrightness = cv::getTrackbarPos("Min Brightness", "Video Mask");
        colorThreshold = cv::getTrackbarPos("Color Difference", "Video Mask");

        cv::Mat blueMask(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
        cv::Mat redMask(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
        cv::Mat mask(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
        //-----开始绘制掩膜------
        for (int row = 0; row < frame.rows; row++) {
            for (int col = 0; col < frame.cols; col++) {
                cv::Vec3b pixel = frame.at<cv::Vec3b>(row, col);

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

        cv::imshow("Video Mask", mask);
        cv::Mat result = frame.clone();
        if (showColorMasks) {
            cv::imshow("Video Blue Color", blueMask);
            cv::imshow("Video Red Color", redMask);
        }
    //-------绘制轮廓并设置面积门槛-------
         std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    double minArea = 50;
    std::vector<std::vector<cv::Point>> filteredContours;

    for (std::size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area < minArea) {
            continue;
        }
        filteredContours.push_back(contours[i]);
    }
    //-------创立灯条，并用矩形框出疑似灯条的轮廓--------
    std::vector<Light> lights;
    std::vector<cv::RotatedRect> lightRects;

    for (std::size_t i = 0; i < filteredContours.size(); i++) {
        cv::RotatedRect rect = cv::minAreaRect(filteredContours[i]);

        double length = std::max(rect.size.width, rect.size.height);
        double width = std::min(rect.size.width, rect.size.height);
        if (width <= 0) {
            continue;
        }
        //-----设置灯条形状门槛--------
        double ratio = length / width;

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

        colorRegion = colorRegion & cv::Rect(0, 0, frame.cols, frame.rows);
        //------color---------
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
    }
    //------降符合条件的灯条开始配对并筛选-------
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
            double minLengthSimilarity = 0.6;
            if(lengthSimilarity<minLengthSimilarity){
                continue;
            }
            double xGap = std::abs(lights[i].getCenterX() - lights[j].getCenterX());
            double yGap = std::abs(lights[i].getCenterY() - lights[j].getCenterY());

            if (xGap <= 0) {
                continue;
            }

            double offsetRatio = yGap / xGap;

            double maxOffsetRatio = 0.25;
            if (offsetRatio > maxOffsetRatio) {
                continue;
            }

            double meanLength = (lights[i].getLength() + lights[j].getLength()) / 2.0;
            if (meanLength < 0) {
                continue;
            }

            double spacingRatio = xGap / meanLength;

            double minSpacingRatio = 1.0;
            double maxSpacingRatio = 4.0;

            if (spacingRatio < minSpacingRatio ||
                spacingRatio > maxSpacingRatio) {
                continue;
            }

            double pairScore = lengthSimilarity - offsetRatio;

            armors.push_back(armor);
            pairedLights[i] = true;
            pairedLights[j] = true;
            cv::Point center(cvRound(armor.getCenterX()), cvRound(armor.getCenterY()));

            cv::Scalar centerColor(0, 0, 255);
            if (lights[i].getColor() == "Blue") {
                centerColor = cv::Scalar(255, 0, 0);
            }
            cv::circle(result, center, 5, centerColor, -1);
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

        cv::imshow("Video",result);

        int key = cv::waitKey(33);
        if(key==27){
            break;
        }
    }
    return 0;
}
