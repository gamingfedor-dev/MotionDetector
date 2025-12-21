#pragma once
#include <opencv2/opencv.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "thread_queue.hpp"

struct TimestampedFrame {
  cv::Mat frame;
  int64_t timestamp_ms;
  uint64_t frame_id;
};

class FrameQueue: public ThreadQueue<TimestampedFrame> {
  public:
    explicit FrameQueue(size_t max_size = 30);
    bool push(TimestampedFrame&& frame) override;
    bool pop(TimestampedFrame& frame, int timeout_ms = 100);
    void shutdown();
    size_t size() const;

  private:
    std::condition_variable cv_not_empty_;
    std::condition_variable cv_not_full_;
    size_t max_size_;
    std::atomic<bool> shutdown_{false};
};