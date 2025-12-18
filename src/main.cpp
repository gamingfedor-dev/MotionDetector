#include "frame_buffer.hpp"
#include "video_capture.hpp"
#include "motion_detector.hpp"
#include "motion_consumer.hpp"
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
    MotionConsumer consumer(buffer, detector);
    
    if (!capture.start()) {
        std::cerr << "Failed to start capture" << std::endl;
        return 1;
    }
    
    consumer.start();
    cv::namedWindow("Motion Detector", cv::WINDOW_AUTOSIZE);

    while (g_running) {
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
