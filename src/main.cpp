#include "queues/frame_queue.hpp"
#include "core/video_capture.hpp"
#include "core/motion_consumer.hpp"
#include "queues/mdcfg_queue.hpp"
#include "queues/mdresult_queue.hpp"
#include "models/detection_result.hpp"
#include <iostream>
#include <csignal>

std::atomic<bool> g_running{true};
void signalHandler(int) { g_running = false; }

// Trackbar values (0-100 scale)
int roi_center_x = 50;
int roi_center_y = 60;
int roi_width = 25;
int roi_height = 60;
int threshold = 25;
int min_contour_area = 10;  // Scaled: actual = value * 100

MotionDetectorConfig buildConfig() {
    MotionDetectorConfig cfg;
    cfg.roi.center_x = roi_center_x / 100.0f;
    cfg.roi.center_y = roi_center_y / 100.0f;
    cfg.roi.width_ratio = std::max(0.05f, roi_width / 100.0f);
    cfg.roi.height_ratio = std::max(0.05f, roi_height / 100.0f);
    cfg.threshold = threshold;
    cfg.min_contour_area = min_contour_area * 100;  // Scale up
    return cfg;
}

int main(int argc, char** argv) {
    std::signal(SIGINT, signalHandler);
    
    std::string source = argc > 1 ? argv[1] : "0";
    std::string output = argc > 2 ? argv[2] : "motion_data.csv";
    
    std::cout << "Motion Detector - Press Q to quit\n";
    
    MotionDetectionConfigQueue config_queue;
    MotionDetectionResultQueue result_queue;
    
    FrameQueue buffer(30);
    VideoCapture capture(source, buffer);
    MotionConsumer consumer(buffer, config_queue, result_queue);
    
    if (!capture.start()) {
        std::cerr << "Failed to start capture" << std::endl;
        return 1;
    }
    
    // Send initial config
    config_queue.push(buildConfig());
    
    consumer.start();
    
    cv::namedWindow("Motion Detector", cv::WINDOW_NORMAL);
    cv::resizeWindow("Motion Detector", 1280, 720);
    cv::moveWindow("Motion Detector", 100, 100);
    
    cv::createTrackbar("ROI X", "Motion Detector", &roi_center_x, 100);
    cv::createTrackbar("ROI Y", "Motion Detector", &roi_center_y, 100);
    cv::createTrackbar("ROI Width", "Motion Detector", &roi_width, 100);
    cv::createTrackbar("ROI Height", "Motion Detector", &roi_height, 100);
    cv::createTrackbar("Threshold", "Motion Detector", &threshold, 100);
    cv::createTrackbar("Min Area", "Motion Detector", &min_contour_area, 300);
    
    MotionDetectorConfig last_config = buildConfig();
    
    while (g_running) {
        // Check if config changed
        MotionDetectorConfig current = buildConfig();
        if (current.roi.center_x != last_config.roi.center_x ||
            current.roi.center_y != last_config.roi.center_y ||
            current.roi.width_ratio != last_config.roi.width_ratio ||
            current.roi.height_ratio != last_config.roi.height_ratio ||
            current.threshold != last_config.threshold ||
            current.min_contour_area != last_config.min_contour_area) {
            
            config_queue.push(std::move(current));
            last_config = std::move(current);
        }
        
        // Display latest result
        if (auto result = result_queue.tryPopLatest()) {
            cv::imshow("Motion Detector", result->visualization);
        }
        
        if (cv::waitKey(1) == 'q') break;
    }
    
    consumer.stop();
    capture.stop();
    consumer.exportCSV(output);
    
    std::cout << "\n=== Final Settings ===\n";
    std::cout << "roi.center_x = " << roi_center_x / 100.0f << "f;\n";
    std::cout << "roi.center_y = " << roi_center_y / 100.0f << "f;\n";
    std::cout << "roi.width_ratio = " << roi_width / 100.0f << "f;\n";
    std::cout << "roi.height_ratio = " << roi_height / 100.0f << "f;\n";
    std::cout << "threshold = " << threshold << ";\n";
    std::cout << "min_contour_area = " << min_contour_area * 100 << ";\n";
    
    std::cout << "Data exported to " << output << std::endl;
    return 0;
}