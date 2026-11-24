#include "armor_detector.hpp"
#include "pnp_solver.hpp"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <stdexcept>

namespace {
using Clock = std::chrono::steady_clock;

void drawLabel(cv::Mat& image, const std::string& label, cv::Point point,
                const cv::Scalar& color = {255, 255, 255}) {
    cv::putText(image, label, point, cv::FONT_HERSHEY_SIMPLEX, 0.65, {0, 0, 0}, 4, cv::LINE_AA);
    cv::putText(image, label, point, cv::FONT_HERSHEY_SIMPLEX, 0.65, color, 1, cv::LINE_AA);
}

void drawArmor(cv::Mat& image, const DetectedArmor& detection,
                const std::optional<PoseResult>& pose, const std::string& targetColor) {
    const cv::Scalar color = targetColor == "Red" ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 120, 0);
    for (int i = 0; i < 4; ++i) {
        cv::line(image, detection.corners[i], detection.corners[(i + 1) % 4], color, 2, cv::LINE_AA);
        cv::circle(image, detection.corners[i], 3, {0, 255, 255}, -1, cv::LINE_AA);
    }
    const cv::Point center(cvRound(detection.armor.getCenterX()), cvRound(detection.armor.getCenterY()));
    cv::circle(image, center, 3, color, -1, cv::LINE_AA);
    const std::string label = targetColor + (pose ? cv::format(" | %.2f m", pose->distance) : " | PnP unavailable");
    drawLabel(image, label, {std::max(0, center.x - 100), std::max(115, center.y - 20)}, color);
}

void prepareOutput(const std::string& path) {
    if (path.empty()) return;
    if (std::filesystem::exists(path)) throw std::runtime_error("Output exists; choose another filename: " + path);
    const auto parent = std::filesystem::path(path).parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent);
}
}  // namespace

int main(int argc, char** argv) {
    const auto root = std::filesystem::path(__FILE__).parent_path();
    const cv::String keys =
        "{help h | | Show usage }"
        "{@video | " + (root / "data/video.avi").string() + " | Input video }"
        "{camera | " + (root / "config/camera.yml").string() + " | Camera calibration }"
        "{color | Red | Red or Blue }"
        "{brightness | 150 | Minimum B,G,R brightness }"
        "{difference | 50 | Minimum red-blue channel difference }"
        "{headless | false | Process without windows }"
        "{tune | false | Show brightness mask and threshold sliders }"
        "{output | | Optional output video (.mp4) }"
        "{csv | | Optional per-frame measurements }"
        "{seconds | 0 | Stop after this many seconds of source video }"
        "{repeat | false | Replay at EOF, only with a positive seconds limit }"
        "{log-every | 30 | Print single-frame time every N frames }";
    cv::CommandLineParser arguments(argc, argv, keys);
    if (arguments.has("help")) { arguments.printMessage(); return 0; }

    try {
        const auto videoPath = arguments.get<std::string>(0);
        const auto cameraPath = arguments.get<std::string>("camera");
        const auto color = arguments.get<std::string>("color");
        const auto output = arguments.get<std::string>("output");
        const auto csvPath = arguments.get<std::string>("csv");
        const bool headless = arguments.get<bool>("headless");
        const bool tune = arguments.get<bool>("tune");
        const bool repeat = arguments.get<bool>("repeat");
        const double seconds = arguments.get<double>("seconds");
        const int logEvery = arguments.get<int>("log-every");
        DetectorParameters parameters;
        parameters.brightness = arguments.get<int>("brightness");
        parameters.colorDifference = arguments.get<int>("difference");
        if (!arguments.check()) { arguments.printErrors(); return 1; }
        if (!std::isfinite(seconds) || seconds < 0 || logEvery <= 0 || (repeat && seconds <= 0) ||
            (headless && tune)) throw std::invalid_argument("Invalid seconds/log interval, or incompatible display options.");
        ArmorDetector detector(color, parameters);
        PnPSolver solver(cameraPath);
        cv::VideoCapture video(videoPath);
        if (!video.isOpened()) throw std::runtime_error("Cannot open video. Run scripts/fetch_course_data.py or supply a video path.");
        const cv::Size size(cvRound(video.get(cv::CAP_PROP_FRAME_WIDTH)), cvRound(video.get(cv::CAP_PROP_FRAME_HEIGHT)));
        if (size != solver.imageSize()) throw std::runtime_error("Video size differs from calibration. Supply a matching camera file.");
        const double fps = video.get(cv::CAP_PROP_FPS);
        if (!std::isfinite(fps) || fps <= 0) throw std::runtime_error("Video FPS is unavailable.");
        if (!output.empty() && output == csvPath) throw std::invalid_argument("Video and CSV outputs must be different.");
        prepareOutput(output);
        prepareOutput(csvPath);
        cv::VideoWriter writer;
        if (!output.empty() && !writer.open(output, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps, size))
            throw std::runtime_error("Cannot create output video.");
        std::ofstream csv;
        if (!csvPath.empty()) {
            csv.open(csvPath);
            if (!csv) throw std::runtime_error("Cannot create CSV.");
            csv << "frame,source_frame,pass,armors,valid_poses,distance_m,reprojection_px,process_ms\n";
            csv << std::fixed << std::setprecision(6);
        }
        if (tune) {
            cv::namedWindow("Task4 Mask", cv::WINDOW_AUTOSIZE);
            cv::createTrackbar("Brightness", "Task4 Mask", nullptr, 255);
            cv::setTrackbarPos("Brightness", "Task4 Mask", parameters.brightness);
            cv::createTrackbar("Color Difference", "Task4 Mask", nullptr, 255);
            cv::setTrackbarPos("Color Difference", "Task4 Mask", parameters.colorDifference);
        }

        int frameCount = 0, sourceFrame = 0, pass = 1, detectedFrames = 0, poseFrames = 0;
        const double frameLimit = seconds > 0 ? std::ceil(seconds * fps) : 0;
        double totalMilliseconds = 0;
        cv::Mat frame;
        while (frameLimit == 0 || frameCount < frameLimit) {
            if (!video.read(frame) || frame.empty()) {
                if (!repeat) break;
                if (!video.set(cv::CAP_PROP_POS_FRAMES, 0) || !video.read(frame) || frame.empty())
                    throw std::runtime_error("Cannot replay video.");
                sourceFrame = 0;
                ++pass;
            }
            ++sourceFrame;
            const auto start = Clock::now();
            if (tune) detector.setThresholds(cv::getTrackbarPos("Brightness", "Task4 Mask"),
                                               cv::getTrackbarPos("Color Difference", "Task4 Mask"));
            cv::Mat mask;
            const auto armors = detector.detect(frame, tune ? &mask : nullptr);
            std::vector<std::optional<PoseResult>> poses;
            int validPoses = 0;
            for (const auto& armor : armors) {
                poses.push_back(solver.solve(armor.corners, frame.size()));
                if (poses.back()) ++validPoses;
            }
            const double milliseconds = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
            totalMilliseconds += milliseconds;
            ++frameCount;
            if (!armors.empty()) ++detectedFrames;
            if (validPoses > 0) ++poseFrames;
            for (std::size_t i = 0; i < armors.size(); ++i) drawArmor(frame, armors[i], poses[i], color);
            drawLabel(frame, cv::format("Task4 | frame %d | source %d | pass %d | %zu armor", frameCount, sourceFrame, pass, armors.size()), {15, 30});
            drawLabel(frame, cv::format("Detection + PnP: %.2f ms | distance: camera to armor center", milliseconds), {15, 60});
            drawLabel(frame, "Calibration: " + std::filesystem::path(cameraPath).filename().string(), {15, 90}, {0, 255, 255});
            if (pass > 1) drawLabel(frame, "REPLAY for timed demonstration", {15, 120}, {0, 255, 255});
            if (csv.is_open()) {
                csv << frameCount << ',' << sourceFrame << ',' << pass << ',' << armors.size() << ',' << validPoses << ',';
                if (!poses.empty() && poses.front()) csv << poses.front()->distance << ',' << poses.front()->reprojectionError;
                else csv << ',';
                csv << ',' << milliseconds << '\n';
            }
            if (frameCount == 1 || frameCount % logEvery == 0)
                std::cout << "Frame " << frameCount << ": " << std::fixed << std::setprecision(2) << milliseconds << " ms\n";
            if (writer.isOpened()) writer.write(frame);
            if (!headless) {
                cv::imshow("Task4 Hello Armor", frame);
                if (tune) cv::imshow("Task4 Mask", mask);
                const double elapsed = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
                const int key = cv::waitKey(std::max(1, cvRound(1000.0 / fps - elapsed))) & 0xff;
                if (key == 27 || key == 'q' || cv::getWindowProperty("Task4 Hello Armor", cv::WND_PROP_VISIBLE) < 1) break;
            }
        }
        if (frameCount == 0) throw std::runtime_error("No video frames decoded.");
        std::cout << "Processed " << frameCount << " frames; armor frames: " << detectedFrames
                  << "; valid PnP frames: " << poseFrames << "; mean processing: "
                  << totalMilliseconds / frameCount << " ms\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
