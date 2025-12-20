#include "frame_buffer.hpp"
#include "video_capture.hpp"
#include "motion_detector.hpp"
#include "motion_consumer.hpp"
#include <iostream>
#include <csignal>


std::atomic<bool> g_running{true};
void signalHandler(int) { g_running = false; }
int roi_center_x = 50;
int roi_center_y = 60;
int roi_width = 25;
int roi_height = 60;
int threshold = 25;

int main(int argc, char** argv) {
    std::signal(SIGINT, signalHandler);
    
    std::string source = argc > 1 ? argv[1] : "0";
    std::string output = argc > 2 ? argv[2] : "motion_data.csv";
    
    std::cout << "Motion Detector - Press Q or Ctrl+C to quit\n";
    
    FrameBuffer buffer(30);
    VideoCapture capture(source, buffer);

    MotionDetector::Config config;
    config.roi.center_x = roi_center_x / 100.0f;
    config.roi.center_y = roi_center_y / 100.0f;
    config.roi.width_ratio = roi_width / 100.0f;
    config.roi.height_ratio = roi_height / 100.0f;

    MotionDetector detector(config);
    MotionConsumer consumer(buffer, detector);
    
    if (!capture.start()) {
        std::cerr << "Failed to start capture" << std::endl;
        return 1;
    }
    
    consumer.start();
    cv::namedWindow("Motion Detector", cv::WINDOW_NORMAL);
    cv::resizeWindow("Motion Detector", 1280, 720);
    cv::moveWindow("Motion Detector", 100, 100);
    cv::waitKey(100);

    cv::createTrackbar("ROI X", "Motion Detector", &roi_center_x, 100);
    cv::createTrackbar("ROI Y", "Motion Detector", &roi_center_y, 100);
    cv::createTrackbar("ROI Width", "Motion Detector", &roi_width, 100);
    cv::createTrackbar("ROI Height", "Motion Detector", &roi_height, 100);
    cv::createTrackbar("Threshold", "Motion Detector", &threshold, 100);

    while (g_running) {
        ROIConfig new_roi;
        new_roi.center_x = roi_center_x / 100.0f;
        new_roi.center_y = roi_center_y / 100.0f;
        new_roi.width_ratio = std::max(0.05f, roi_width / 100.0f);  // Min 5%
        new_roi.height_ratio = std::max(0.05f, roi_height / 100.0f);
        detector.setROI(new_roi);

        cv::Mat viz;
        MotionEvent event;
        
        if (consumer.getLatestVisualizationFrame(viz, event)) {
            cv::imshow("Motion Detector", viz);
        }
        
        if (cv::waitKey(1) == 'q') break;
    }
    
    consumer.stop();
    capture.stop();
    detector.exportCSV(output);
    std::cout << "Data exported to " << output << std::endl;
    return 0;
}
