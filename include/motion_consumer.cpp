#include "motion_consumer.hpp"

MotionConsumer::MotionConsumer(FrameBuffer& buffer_, MotionDetector& detector)
    : buffer_(buffer_), detector_(detector) {
}

MotionConsumer::~MotionConsumer() {
    stop();
}

void MotionConsumer::start() {
    running_ = true;
    processing_thread_ = std::thread(&MotionConsumer::processLoop, this);
}

void MotionConsumer::stop() {
    running_ = false;
    if (processing_thread_.joinable()) {
        processing_thread_.join();
    }
}

void MotionConsumer::processLoop() {
  while (running_) {
          TimestampedFrame tf;
          if (!buffer_.pop(tf, 100)) continue;
          
          auto event = detector_.process(tf.frame, tf.frame_id, tf.timestamp_ms);
          
          cv::Mat viz = detector_.getVisualization();
          char stats[128];
          snprintf(stats, sizeof(stats),
              "Motion: %.2f%% | Objects: %d | Frame: %lu",
              event.motion_score, event.contour_count, tf.frame_id);
          cv::putText(viz, stats, cv::Point(10, 30),
              cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
          
          {
            std::lock_guard<std::mutex> lock(viz_mutex_);
            latest_viz_ = viz;
            latest_event_ = event;
            has_new_frame_ = true;
          }
  }
}

bool MotionConsumer::getLatestVisualizationFrame(cv::Mat& viz, MotionEvent& event) {
    std::lock_guard<std::mutex> lock(viz_mutex_);
    if (!has_new_frame_) return false;
    cv::swap(viz, latest_viz_);
    event = latest_event_;
    has_new_frame_ = false;
    return true;
}