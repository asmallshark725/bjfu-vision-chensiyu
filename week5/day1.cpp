#include <iostream>
#include <opencv2/opencv.hpp>
int main(){
    cv::Mat image = cv::imread("images/test.jpg", cv::IMREAD_COLOR);
    if (image.empty()) {
    std::cout << "图片读取失败" << std::endl;
    return 1;
}
    std::cout << "图片读取成功" << std::endl;
    std::cout << "宽度：" << image.cols << std::endl;
    std::cout << "高度：" << image.rows << std::endl;
    std::cout << "通道数：" << image.channels() << std::endl;

    cv::imshow("Day1", image);
    cv::waitKey(0);
    return 0;
}