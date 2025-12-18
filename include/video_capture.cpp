#include "video_capture.hpp"
#include <chrono>
#include <iostream>

using namespace std::chrono;

VideoCapture::VideoCapture(const std::string& source, FrameBuffer& buffer)
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
    while (running_) {
        cv::Mat frame;
        if (!cap_.read(frame) || frame.empty()) {
            if (source_type_ == SourceType::RTSP ||
                source_type_ == SourceType::HTTP) {
                std::this_thread::sleep_for(seconds(1));
                cap_.open(source_);
                continue;
            }
            break;
        }
        
        auto now = steady_clock::now();
        auto ts = duration_cast<milliseconds>(
            now.time_since_epoch()).count();
        
        buffer_.push(TimestampedFrame{
            std::move(frame), ts, frame_count_++});
    }
}

void VideoCapture::stop() {
    running_ = false;
    buffer_.shutdown();
    if (capture_thread_.joinable())
        capture_thread_.join();
    cap_.release();
}