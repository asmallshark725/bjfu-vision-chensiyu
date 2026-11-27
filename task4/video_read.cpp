#include "armor.hpp"
#include "light.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) {
    // 1. 打开视频；第一个命令行参数可指定视频路径。
    std::string videoPath = "/mnt/e/robomaster/school_training/hello_armor/video.avi";
    if (argc > 1) {
        videoPath = argv[1];
    }
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cout << "视频打开失败: " << videoPath << std::endl;
        return 1;
    }

    cv::Mat frame;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    int colorThreshold = 50;
    bool showColorMasks = false;
    std::string targetColor;
    std::cout << "输入目标颜色(Red / Blue):";
    std::cin >> targetColor;
    if (targetColor != "Red" && targetColor != "Blue") {
        std::cout << "目标颜色只能是 Red 或 Blue" << std::endl;
        return 1;
    }

    // 2. 创建调参窗口；这些初始化只在播放前执行一次。
    int minBrightness = 200;
    cv::namedWindow("Video Mask", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Min Brightness", "Video Mask", nullptr, 255);
    cv::setTrackbarPos("Min Brightness", "Video Mask", minBrightness);
    cv::createTrackbar("Color Difference", "Video Mask", nullptr, 255);
    cv::setTrackbarPos("Color Difference", "Video Mask", colorThreshold);

    // 当前课程视频配套的相机参数，在循环外初始化一次。
    double cx = 645.34450819155256;
    double cy = 483.6163720308021;
    double fx = 1286.307063384126;
    double fy = 1288.1400736562441;
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    std::cout << "cameraMatrix:\n" << cameraMatrix << std::endl;

    // 畸变系数按 k1、k2、p1、p2、k3 的顺序保存为 1 行 5 列。
    cv::Mat distCoeffs = (cv::Mat_<double>(1, 5) << -0.47562935060124745, // k1
                          0.21831745829617311,                            // k2
                          0.0004957613589406044,                          // p1
                          -0.00034617769548693592,                        // p2
                          0.0                                             // k3
    );
    std::cout << "distCoeffs:\n" << distCoeffs << std::endl;
    double armorWidth = 0.135;
    double lightLength = 0.056;
    std::vector<cv::Point3f> objectPoints;
    objectPoints.push_back(cv::Point3f(-armorWidth / 2.0, -lightLength / 2.0, 0.0));
    objectPoints.push_back(cv::Point3f(armorWidth / 2.0, -lightLength / 2.0, 0.0));
    objectPoints.push_back(cv::Point3f(armorWidth / 2.0, lightLength / 2.0, 0.0));
    objectPoints.push_back(cv::Point3f(-armorWidth / 2.0, lightLength / 2.0, 0.0));
    bool printedPose = false;
    // 每轮循环处理一帧，帧内的容器重新创建，不保留上一帧的检测结果。
    while (true) {
        if (!cap.read(frame)) {
            std::cout << "视频结束或读取失败" << std::endl;
            break;
        }
        minBrightness = cv::getTrackbarPos("Min Brightness", "Video Mask");
        colorThreshold = cv::getTrackbarPos("Color Difference", "Video Mask");
        int64 startTick = cv::getTickCount();

        // 3. 生成颜色掩膜和亮度掩膜。
        cv::Mat blueMask(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
        cv::Mat redMask(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
        cv::Mat mask(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
        cv::Mat closedMask;

        // OpenCV 默认通道顺序为 B、G、R。
        cv::Mat channels[3];
        cv::split(frame, channels);
        cv::Mat blueDifference;
        cv::Mat redDifference;
        cv::subtract(channels[0], channels[2], blueDifference);
        cv::subtract(channels[2], channels[0], redDifference);
        cv::inRange(blueDifference, cv::Scalar(colorThreshold + 1), cv::Scalar(255), blueMask);
        cv::inRange(redDifference, cv::Scalar(colorThreshold + 1), cv::Scalar(255), redMask);
        cv::inRange(frame, cv::Scalar(minBrightness, minBrightness, minBrightness),
                    cv::Scalar(255, 255, 255), mask);
        cv::imshow("Video Mask", mask);
        cv::Mat result = frame.clone();
        cv::Point2f mainpoint(cx, cy);
        cv::circle(result, mainpoint, 5, cv::Scalar(0, 255, 0), -1);
        if (showColorMasks) {
            cv::imshow("Video Blue Color", blueMask);
            cv::imshow("Video Red Color", redMask);
        }

        // 可选闭运算：保留学习记录，当前未启用。
        // cv::morphologyEx(mask, closedMask, cv::MORPH_CLOSE, kernel);
        // cv::imshow("Closed Mask", closedMask);

        // 4. 从亮度掩膜寻找轮廓，先排除面积过小的轮廓。
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        double minArea = 25;
        std::vector<std::vector<cv::Point>> filteredContours;

        for (std::size_t i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]);
            if (area < minArea) {
                continue;
            }
            filteredContours.push_back(contours[i]);
        }

        // 5. 筛选单根灯条：形状合格，再统计附近的颜色。
        std::vector<Light> lights;
        std::vector<cv::RotatedRect> lightRects;

        for (std::size_t i = 0; i < filteredContours.size(); i++) {
            cv::RotatedRect rect = cv::minAreaRect(filteredContours[i]);

            double length = std::max(rect.size.width, rect.size.height);
            double width = std::min(rect.size.width, rect.size.height);
            if (width <= 0) {
                continue;
            }

            // 5.1 计算长宽比，以及统一朝上的长轴方向。
            double ratio = length / width;

            cv::Point2f vertices[4];
            rect.points(vertices);

            cv::Point2f edge1 = vertices[1] - vertices[0];
            cv::Point2f edge2 = vertices[2] - vertices[1];
            cv::Point2f longEdge = edge1;
            if (cv::norm(edge2) > cv::norm(edge1)) {
                longEdge = edge2;
            }

            // 图像 y 轴向下；统一方向后，“加半个灯长”就能得到上端点。
            if (longEdge.y > 0) {
                longEdge.x = -longEdge.x;
                longEdge.y = -longEdge.y;
            }

            double angle = std::atan2(longEdge.x, -longEdge.y) * 180.0 / CV_PI;
            double tilt = std::atan2(std::abs(longEdge.x), std::abs(longEdge.y)) * 180.0 / CV_PI;
            double maxTilt = 60.0;
            double minAspectRatio = 2.0;
            double maxAspectRatio = 15.0;

            if (tilt > maxTilt) {
                continue;
            }
            if (ratio < minAspectRatio || ratio > maxAspectRatio) {
                continue;
            }

            // 5.2 扩大颜色统计范围，并裁剪到图像内部。
            cv::RotatedRect sampleRect = rect;

            if (sampleRect.size.width < sampleRect.size.height) {
                sampleRect.size.width *= 3.0f;
                sampleRect.size.height *= 1.1f;
            } else {
                sampleRect.size.width *= 1.1f;
                sampleRect.size.height *= 3.0f;
            }

            // 1. 截取旋转采样区域的水平外框，并限制在图像内。
            cv::Rect colorRegion = sampleRect.boundingRect();
            colorRegion = colorRegion & cv::Rect(0, 0, frame.cols, frame.rows);
            if (colorRegion.empty()) {
                continue;
            }

            // 2. 取得旋转矩形四角，换算成局部图像中的坐标。
            cv::Point2f sampleVertices[4];
            sampleRect.points(sampleVertices);

            std::vector<cv::Point> polygon;
            for (int k = 0; k < 4; k++) {
                int x = cvRound(sampleVertices[k].x) - colorRegion.x;
                int y = cvRound(sampleVertices[k].y) - colorRegion.y;
                polygon.push_back(cv::Point(x, y));
            }

            // 3. 创建黑色掩膜，再把旋转矩形内部填成白色。
            cv::Mat sampleMask(colorRegion.size(), CV_8UC1, cv::Scalar(0));
            cv::fillConvexPoly(sampleMask, polygon, cv::Scalar(255));

            // 4. 同时满足“颜色合格”和“位于采样区域内”，才保留为白色。
            cv::Mat selectedBlue;
            cv::Mat selectedRed;
            cv::bitwise_and(blueMask(colorRegion), sampleMask, selectedBlue);
            cv::bitwise_and(redMask(colorRegion), sampleMask, selectedRed);

            int blueCount = cv::countNonZero(selectedBlue);
            int redCount = cv::countNonZero(selectedRed);
            std::string colorName = "Unknown";
            int colorCount = blueCount + redCount;
            if (colorCount >= 5) {
                double blueShare = 1.0 * blueCount / colorCount;
                if (blueShare >= 0.6) {
                    colorName = "Blue";
                } else if (blueShare <= 0.4) {
                    colorName = "Red";
                }
            }

            if (colorName == "Unknown") {
                continue;
            }
            if (colorName != targetColor) {
                continue;
            }

            Light light(rect.center.x, rect.center.y, length, width, angle);
            light.setColor(colorName);

            lights.push_back(light);
            lightRects.push_back(rect);
        }

        // 6. 将合格灯条两两配对，判断能否组成装甲板。
        std::vector<Armor> armors;
        std::vector<bool> pairedLights(lights.size(), false);

        for (std::size_t i = 0; i < lights.size(); i++) {
            for (std::size_t j = i + 1; j < lights.size(); j++) {
                // 6.1 检查颜色、两灯角度差和长度相似度。
                if (lights[i].getColor() != lights[j].getColor()) {
                    continue;
                }

                Armor armor(lights[i], lights[j]);
                if (!armor.isParallel(22.0)) {
                    continue;
                }
                double shorterLength = std::min(lights[i].getLength(), lights[j].getLength());
                double longerLength = std::max(lights[i].getLength(), lights[j].getLength());
                double lengthSimilarity = shorterLength / longerLength;
                double minLengthSimilarity = 0.6;
                if (lengthSimilarity < minLengthSimilarity) {
                    continue;
                }

                // 6.2 从已保存的角度恢复两根灯条的单位方向。
                double angleI = lights[i].getangle() * CV_PI / 180.0;
                double angleJ = lights[j].getangle() * CV_PI / 180.0;
                cv::Point2f directionI(std::sin(angleI), -std::cos(angleI));
                cv::Point2f directionJ(std::sin(angleJ), -std::cos(angleJ));

                // 两个方向相加后归一化，得到共同长轴及其垂直方向。
                cv::Point2f directionSum = directionI + directionJ;
                double directionLength = cv::norm(directionSum);
                if (directionLength < 1e-6) {
                    continue;
                }
                cv::Point2f lightAxis = directionSum / directionLength;
                cv::Point2f acrossAxis(-lightAxis.y, lightAxis.x);
                cv::Point2f centerDelta(lights[j].getCenterX() - lights[i].getCenterX(),
                                        lights[j].getCenterY() - lights[i].getCenterY());

                // 6.3 用点积测量沿灯条的错位、横跨灯条的间距。
                double alongGap = std::abs(centerDelta.dot(lightAxis));
                double acrossGap = std::abs(centerDelta.dot(acrossAxis));
                if (acrossGap <= 1e-6) {
                    continue;
                }
                double offsetRatio = alongGap / acrossGap;
                double maxOffsetRatio = 0.4;
                if (offsetRatio > maxOffsetRatio) {
                    continue;
                }
                double meanLength = (lights[i].getLength() + lights[j].getLength()) / 2.0;
                if (meanLength < 1e-6) {
                    continue;
                }
                double spacingRatio = acrossGap / meanLength;
                double minSpacingRatio = 1.0;
                double maxSpacingRatio = 4.0;
                if (spacingRatio < minSpacingRatio || spacingRatio > maxSpacingRatio) {
                    continue;
                }

                // 当前只计算配对分数，尚未用于候选选优。
                double pairScore = lengthSimilarity - offsetRatio;

                // 6.4 求两灯条的上、下端点，再统一四点顺序。
                cv::Point2f centerI(lights[i].getCenterX(), lights[i].getCenterY());
                cv::Point2f topI = centerI + directionI * (lights[i].getLength() / 2.0);
                cv::Point2f bottomI = centerI - directionI * (lights[i].getLength() / 2.0);

                cv::Point2f centerJ(lights[j].getCenterX(), lights[j].getCenterY());
                cv::Point2f topJ = centerJ + directionJ * (lights[j].getLength() / 2.0);
                cv::Point2f bottomJ = centerJ - directionJ * (lights[j].getLength() / 2.0);

                // 固定顺序：0 左上、1 右上、2 右下、3 左下。
                // 两个分支都会为四个元素赋值，因此这里不需要重复初始化。
                cv::Point2f endpoints[4];
                if (centerI.x < centerJ.x) {
                    // i 在左，j 在右。
                    endpoints[0] = topI;
                    endpoints[1] = topJ;
                    endpoints[2] = bottomJ;
                    endpoints[3] = bottomI;
                } else {
                    // j 在左，i 在右。
                    endpoints[0] = topJ;
                    endpoints[1] = topI;
                    endpoints[2] = bottomI;
                    endpoints[3] = bottomJ;
                }
                std::vector<cv::Point2f> imagePoints;
                for (int k = 0; k < 4; k++) {
                    imagePoints.push_back(endpoints[k]);
                }
                cv::Mat rvec;
                cv::Mat tvec;
                bool poseSolved =
                    cv::solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);
                if (poseSolved) {
                    double depth = tvec.at<double>(2, 0);
                    double distance = cv::norm(tvec);
                    std::vector<cv::Point2f> reprojectedPoints;
                    cv::projectPoints(objectPoints, rvec, tvec, cameraMatrix, distCoeffs,
                                      reprojectedPoints);
                    if (depth > 0.0) {
                        std::string distanceTect = cv::format("D:%.2f m", distance);
                        cv::Point2f textPosition = endpoints[0] + cv::Point2f(0, -20);
                        cv::putText(result, distanceTect, textPosition, cv::FONT_HERSHEY_SIMPLEX,
                                    0.5, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
                    }
                    double errorSum = 0.0;
                    for (int k = 0; k < 4; k++) {
                        cv::circle(result, reprojectedPoints[k], 8, cv::Scalar(0, 255, 255), 2);
                        errorSum += cv::norm(reprojectedPoints[k] - imagePoints[k]);
                    }
                    double meanReprojectionError = errorSum / 4.0;
                    if (!printedPose) {
                        std::cout << "Depth(m):" << depth << "\nDistance(m):" << distance
                                  << std::endl;
                        std::cout << "Mean reprojection error (px):" << meanReprojectionError
                                  << std::endl;
                        printedPose = true;
                    }
                }
                // 6.5 记录配对结果，并绘制中心、四个端点及其编号。
                armors.push_back(armor);
                pairedLights[i] = true;
                pairedLights[j] = true;
                cv::Point center(cvRound(armor.getCenterX()), cvRound(armor.getCenterY()));

                cv::Scalar centerColor(0, 0, 255);
                if (lights[i].getColor() == "Blue") {
                    centerColor = cv::Scalar(255, 0, 0);
                }
                cv::circle(result, center, 5, centerColor, -1);

                for (int k = 0; k < 4; k++) {
                    cv::circle(result, endpoints[k], 4, cv::Scalar(0, 0, 255), -1);

                    cv::putText(result, std::to_string(k), endpoints[k] + cv::Point2f(6, -6),
                                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1,
                                cv::LINE_AA);
                }
            }
        }

        // 7. 绘制成功配对的灯条外框与颜色标签。
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
                cv::line(result, vertices[j], vertices[(j + 1) % 4], lightColor, 2);
            }

            cv::Rect bounds = lightRects[i].boundingRect();
            cv::Point labelPoint(std::max(0, bounds.x), std::max(12, bounds.y - 5));
            cv::putText(result, lights[i].getColor(), labelPoint, cv::FONT_HERSHEY_SIMPLEX, 0.45,
                        lightColor, 1, cv::LINE_AA);
        }

        // 8. 显示当前帧；等待键盘输入，按 Esc 退出。
        int64 endTick = cv::getTickCount();
        cv::imshow("Video", result);
        double elapsedMs = (endTick - startTick) / cv::getTickFrequency() * 1000.0;
        std::cout << "Frame processing: " << elapsedMs << " ms" << std::endl;
        int key = cv::waitKey(33);
        if (key == 27) {
            break;
        }
    }
    return 0;
}
