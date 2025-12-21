#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <atomic>
#include "frame_queue.hpp"

class VideoCapture {
public:
    enum class SourceType { CAMERA, FILE, RTSP, HTTP };
    
    VideoCapture(const std::string& source, FrameQueue& buffer);
    ~VideoCapture();
    
    bool start();
    void stop();
    bool isRunning() const { return running_; }
    
private:
    void captureLoop();
    SourceType detectSourceType(const std::string& source);
    
    std::string source_;
    SourceType source_type_;
    FrameQueue& buffer_;
    cv::VideoCapture cap_;
    std::thread capture_thread_;
    std::atomic<bool> running_{false};
    std::atomic<uint64_t> frame_count_{0};
};