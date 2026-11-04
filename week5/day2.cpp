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
    int x = 10;
    int y = 20;

    cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);

    int blue = pixel[0];
    int green = pixel[1];
    int red = pixel[2];

    std::cout << "B:" << blue << std::endl;
    std::cout << "G:" << green << std::endl;
    std::cout << "R:" << red << std::endl;
    for(int row=50;row<100;row++){
        for(int col=50;col<100;col++){
            image.at<cv::Vec3b>(row,col)=cv::Vec3b(0,0,225);
        }
    }
    bool saved = cv::imwrite("images/day2_result.png", image);
    if (saved) {
    std::cout << "结果图片保存成功" << std::endl;
    }   
    cv::imshow("Day2", image);
    cv::waitKey(0);
    return 0;
}
