#include "frame_buffer.hpp"
#include "video_capture.hpp"
#include "motion_detector.hpp"
#include <iostream>
#include <csignal>


std::atomic<bool> g_running{true};
void signalHandler(int) { g_running = false; }


int main(int argc, char** argv) {
    std::signal(SIGINT, signalHandler);
    
    std::string source = argc > 1 ? argv[1] : "0";
    std::string output = argc > 2 ? argv[2] : "motion_data.csv";
    
    std::cout << "Motion Detector - Press Q or Ctrl+C to quit\n";
    
    FrameBuffer buffer(30);
    VideoCapture capture(source, buffer);
    MotionDetector detector;
    
    if (!capture.start()) {
        std::cerr << "Failed to start capture" << std::endl;
        return 1;
    }
    
    cv::namedWindow("Motion Detector", cv::WINDOW_AUTOSIZE);
    
    while (g_running) {
        TimestampedFrame tf;
        if (!buffer.pop(tf, 100)) continue;
        
        auto event = detector.process(tf.frame, tf.frame_id, tf.timestamp_ms);
        
        cv::Mat viz = detector.getVisualization();
        char stats[128];
        snprintf(stats, sizeof(stats),
            "Motion: %.2f%% | Objects: %d | Frame: %lu",
            event.motion_score, event.contour_count, tf.frame_id);
        cv::putText(viz, stats, cv::Point(10, 30),
            cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
        
        cv::imshow("Motion Detector", viz);
        if (cv::waitKey(1) == 'q') break;
    }
    
    capture.stop();
    detector.exportCSV(output);
    std::cout << "Data exported to " << output << std::endl;
    return 0;
}
