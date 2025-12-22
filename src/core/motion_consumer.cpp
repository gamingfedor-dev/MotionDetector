#include "motion_consumer.hpp"

MotionConsumer::MotionConsumer(FrameQueue& buffer,
                               ThreadQueue<MotionDetectorConfig>& config_queue,
                               ThreadQueue<DetectionResult>& result_queue)
    : buffer_(buffer)
    , config_queue_(config_queue)
    , result_queue_(result_queue)
    , detector_() {}

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

void MotionConsumer::applyConfig(const MotionDetectorConfig& config) {
    MotionDetector::Config motion_detector_cfg;
    motion_detector_cfg.roi = config.roi;
    motion_detector_cfg.threshold = config.threshold;
    motion_detector_cfg.blur_kernel = config.blur_kernel;
    motion_detector_cfg.min_contour_area = config.min_contour_area;
    motion_detector_cfg.learning_rate = config.learning_rate;
    
    detector_.setConfig(motion_detector_cfg);
}

void MotionConsumer::processLoop() {
    while (running_) {
        // Check for config updates
        if (auto config = config_queue_.tryPopLatest()) {
            applyConfig(*config);
        }
        
        // Get frame
        TimestampedFrame tf;
        if (!buffer_.pop(tf, 100)) continue;
        
        // Process
        auto event = detector_.process(tf.frame, tf.frame_id, tf.timestamp_ms);
        cv::Mat viz = detector_.getVisualization();
        
        // Add overlay
        char stats[128];
        snprintf(stats, sizeof(stats),
            "Motion: %.2f%% | Objects: %d | Frame: %lu",
            event.motion_score, event.contour_count, tf.frame_id);
        cv::putText(viz, stats, cv::Point(10, 30),
            cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);

        result_queue_.push(DetectionResult{std::move(viz), event});
    }
}

void MotionConsumer::exportCSV(const std::string& path) const {
    detector_.exportCSV(path);
}