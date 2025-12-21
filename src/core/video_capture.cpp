#include "video_capture.hpp"
#include <chrono>
#include <iostream>

using namespace std::chrono;

VideoCapture::VideoCapture(const std::string& source, FrameQueue& buffer)
    : source_(source), buffer_(buffer) {
    source_type_ = detectSourceType(source);
}

VideoCapture::~VideoCapture() { stop(); }

VideoCapture::SourceType VideoCapture::detectSourceType(const std::string& source) {
    if (source.find("rtsp://") == 0)
        return SourceType::RTSP;
    if (source.find("http://") == 0 || source.find("https://") == 0)
        return SourceType::HTTP;
    try {
        std::stoi(source);
        return SourceType::CAMERA;
    } catch (...) {
        return SourceType::FILE;
    }
}

bool VideoCapture::start() {
    if (source_type_ == SourceType::CAMERA) {
        cap_.open(std::stoi(source_));
    } else {
        cap_.open(source_);
    }
    
    if (!cap_.isOpened()) {
        std::cerr << "Failed to open: " << source_ << std::endl;
        return false;
    }
    
    running_ = true;
    capture_thread_ = std::thread(&VideoCapture::captureLoop, this);
    return true;
}

void VideoCapture::captureLoop() {
 double fps = cap_.get(cv::CAP_PROP_FPS);
    if (fps <= 0) fps = 30.0;
    
    auto frame_duration = std::chrono::milliseconds(static_cast<int>(1000.0 / fps));
    
    while (running_) {
        auto frame_start = std::chrono::steady_clock::now();
        
        cv::Mat frame;
        if (!cap_.read(frame) || frame.empty()) {
            if (source_type_ == SourceType::RTSP ||
                source_type_ == SourceType::HTTP) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                cap_.open(source_);
                continue;
            }
            
            if (source_type_ == SourceType::FILE) {
                cap_.set(cv::CAP_PROP_POS_FRAMES, 0);
                continue;
            }
            
            break;
        }
        
        auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        
        buffer_.push(TimestampedFrame{std::move(frame), ts, frame_count_++});
        
        // Sleep to maintain FPS
        auto elapsed = std::chrono::steady_clock::now() - frame_start;
        if (elapsed < frame_duration) {
            std::this_thread::sleep_for(frame_duration - elapsed);
        }
    }
}

void VideoCapture::stop() {
    running_ = false;
    buffer_.shutdown();
    if (capture_thread_.joinable())
        capture_thread_.join();
    cap_.release();
}