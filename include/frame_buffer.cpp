#include "frame_buffer.hpp"
#include <chrono>

FrameBuffer::FrameBuffer(size_t max_size) : max_size_(max_size) {}

bool FrameBuffer::push(TimestampedFrame&& frame) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_not_full_.wait(lock, [this] {
      return buffer_.size() < max_size_ || shutdown_;
    });
    if (shutdown_) return false;
    buffer_.push(std::move(frame));
    cv_not_empty_.notify_one();
    return true;
}

bool FrameBuffer::pop(TimestampedFrame& frame, int timeout_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    bool has_data = cv_not_empty_.wait_for(
        lock, std::chrono::milliseconds(timeout_ms),
        [this] { return !buffer_.empty() || shutdown_; }
    );
    if (!has_data || buffer_.empty()) return false;
    frame = std::move(buffer_.front());
    buffer_.pop();
    cv_not_full_.notify_one();
    return true;
}

void FrameBuffer::shutdown() {
    shutdown_ = true;
    cv_not_empty_.notify_all();
    cv_not_full_.notify_all();
}


size_t FrameBuffer::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
}

