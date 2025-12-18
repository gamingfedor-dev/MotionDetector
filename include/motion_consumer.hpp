#pragma once
#include "motion_detector.hpp"
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include "frame_buffer.hpp"


class MotionConsumer {
  public:
    explicit MotionConsumer(FrameBuffer& buffer_, MotionDetector& detector);
    ~MotionConsumer();
    void start();
    void stop();
    bool getLatestVisualizationFrame(cv::Mat& viz, MotionEvent& event); 
  private:
    void processLoop();
    
    FrameBuffer& buffer_;
    MotionDetector& detector_;
    std::thread processing_thread_;
    std::atomic<bool> running_{false};
    
    // Thread-safe latest result
    std::mutex viz_mutex_;
    cv::Mat latest_viz_;
    MotionEvent latest_event_;
    bool has_new_frame_{false};

};
