#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    // 1. 按彩色模式读取原图
    cv::Mat image = cv::imread("images/test.jpg", cv::IMREAD_COLOR);

    // 2. 检查图片是否读取成功
    if (image.empty()) {
        std::cout << "图片读取失败" << std::endl;
        return 1;
    }

    // 3. 设置判断蓝色的阈值
    const int threshold = 80;
    const int minBlue = 200;

    // 4. 创建与原图同宽、同高的单通道掩膜
    // CV_8UC1：8位无符号数，1个通道
    // cv::Scalar(0)：所有像素初始化为0，即黑色
    cv::Mat mask(image.rows, image.cols, CV_8UC1, cv::Scalar(0));

    // 5. 外层循环遍历每一行
    for (int row = 0; row < image.rows; row++) {

        // 6. 内层循环遍历这一行中的每一列
        for (int col = 0; col < image.cols; col++) {

            // 7. 读取当前位置的B、G、R三个值
            cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);

            // 8. 取出蓝色和红色通道值
            int blue = pixel[0];
            int red = pixel[2];

            // 9. 蓝色值比红色值大80以上，就标记为白色
            if (blue - red > threshold && blue > minBlue) {
    mask.at<unsigned char>(row, col) = 255;
}
            // 不满足条件的位置，保持初始化时的黑色0
        }
    }

    // 10. 将掩膜保存成PNG图片
    bool saved = cv::imwrite("images/day3_mask_80_b200.png", mask);

    // 11. 检查保存结果
    if (saved) {
        std::cout << "掩膜保存成功" << std::endl;
    } else {
        std::cout << "掩膜保存失败" << std::endl;
        return 1;
    }

    // 12. 分别显示原图和掩膜
    cv::imshow("Day3 Original", image);
    cv::imshow("Day3 Mask", mask);

    // 13. 等待图片窗口中的按键
    cv::waitKey(0);

    return 0;
}