#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>
int main() {
    cv::Mat image = cv::imread("images/test.jpg", cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cout << "图片读取失败" << std::endl;
        return 1;
    }

    int threshold = 80;
    int minBlue = 200;

    cv::Mat mask(image.rows, image.cols, CV_8UC1, cv::Scalar(0));

    // 先创建窗口，再创建窗口中的滑动条
    cv::namedWindow("Day5 Mask", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("B-R", "Day5 Mask", nullptr, 255);
    cv::createTrackbar("Min B", "Day5 Mask", nullptr, 255);
    cv::setTrackbarPos("B-R", "Day5 Mask", threshold);
    cv::setTrackbarPos("Min B", "Day5 Mask", minBlue);

    cv::imshow("Day5 Original", image);

    while (true) {
        threshold = cv::getTrackbarPos("B-R","Day5 Mask");
        minBlue = cv::getTrackbarPos("Min B","Day5 Mask");
        // 每轮重新计算前，清除上一次的结果
        mask.setTo(0);

        for (int row = 0; row < image.rows; row++) {
            for (int col = 0; col < image.cols; col++) {
                cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);

                int blue = pixel[0];
                int red = pixel[2];

                if (blue - red > threshold && blue > minBlue) {
                    mask.at<unsigned char>(row, col) = 255;
                }
            }
        }

        cv::imshow("Day5 Mask", mask);

        // 处理窗口事件，按 Esc 退出循环
        int key = cv::waitKey(30);
        if (key == 27) {
            break;
        }
    }
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
    std::cout << "轮廓数量：" << contours.size() << std::endl;
    cv::Mat result = image.clone();
    cv::drawContours(result, contours, -1, cv::Scalar(255, 0, 255), 2);
    cv::imshow("Day5 Contours", result);
    cv::imwrite("images/day5_contours.png",result);
    cv::imshow("Day5 Contours", result);
    cv::waitKey(0);
    // 退出循环后，保存最后一次计算的掩膜
    bool saved = cv::imwrite("images/day5_mask.png", mask);

    if (saved) {
        std::cout << "掩膜保存成功" << std::endl;
    } else {
        std::cout << "掩膜保存失败" << std::endl;
        return 1;
    }

    std::cout << "最终色差阈值：" << threshold << std::endl;
    std::cout << "最低蓝色值：" << minBlue << std::endl;

    return 0;
}
