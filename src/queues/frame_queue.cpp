#include "queues/frame_queue.hpp"
#include <chrono>

FrameQueue::FrameQueue(size_t max_size) : max_size_(max_size) {}
/** 
 * Push frame to buffer, blocking if queue is full
 * 
 * Waits until there's space (or shutdown), then pushes frame
 * and notifies any waiting pop() calls.
 * Returns false if shutdown during wait, true on success
*/
bool FrameQueue::push(TimestampedFrame&& frame) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_not_full_.wait(lock, [this] {
      return queue_.size() < max_size_ || shutdown_;
    });
    if (shutdown_) return false;
    queue_.push(std::move(frame));
    cv_not_empty_.notify_one();
    return true;
}

/** 
 * Pop frame from buffer, blocking up to timeout if empty
 * 
 * Waits until there's data (or shutdown/timeout), then pops frame
 * and notifies any waiting push() calls.
 * Returns false if timeout/shutdown with no data, true on success
*/
bool FrameQueue::pop(TimestampedFrame& frame, int timeout_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    bool has_data = cv_not_empty_.wait_for(
        lock, std::chrono::milliseconds(timeout_ms),
        [this] { return !queue_.empty() || shutdown_; }
    );
    if (!has_data || queue_.empty()) return false;
    frame = std::move(queue_.front());
    queue_.pop();
    cv_not_full_.notify_one();
    return true;
}

/** Shutdown the buffer, waking up all waiting threads */
void FrameQueue::shutdown() {
    shutdown_ = true;
    cv_not_empty_.notify_all();
    cv_not_full_.notify_all();
}

    
size_t FrameQueue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

