#pragma once
#include "motion_detector.hpp"
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include "../queues/frame_queue.hpp"
#include "../queues/thread_queue.hpp"
#include "../models/mdcfg.hpp"
#include "../models/detection_result.hpp"

class MotionConsumer {
public:
    MotionConsumer(FrameQueue& buffer,
                   ThreadQueue<MotionDetectorConfig>& config_queue,
                   ThreadQueue<DetectionResult>& result_queue);
    ~MotionConsumer();
    void start();
    void stop();
    void exportCSV(const std::string& path) const;
    
private:
    void processLoop();
    void applyConfig(const MotionDetectorConfig& config);
    
    FrameQueue& buffer_;
    ThreadQueue<MotionDetectorConfig>& config_queue_;
    ThreadQueue<DetectionResult>& result_queue_;
    
    MotionDetector detector_;  // Owned, not reference
    std::thread processing_thread_;
    std::atomic<bool> running_{false};
};