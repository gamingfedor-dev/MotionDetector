#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include "roi_config.hpp"

struct MotionEvent {
    uint64_t frame_id;
    int64_t timestamp_ms;
    double motion_score;
    int contour_count;
    cv::Rect largest_bbox;
    cv::Rect roi_used;
};

class MotionDetector {
public:
    struct Config {
        int blur_kernel = 21;
        int threshold = 25;
        int min_contour_area = 500;
        double learning_rate = 0.01;
        ROIConfig roi;
        bool draw_roi = true;
    };
    explicit MotionDetector();
    explicit MotionDetector(const Config& cfg);

    void setROI(const ROIConfig& roi);
    ROIConfig getROI() const;

    MotionEvent process(const cv::Mat& frame, uint64_t id, int64_t ts);
    cv::Mat getVisualization() const;
    void exportCSV(const std::string& path) const;
    
private:
    Config config_;
    cv::Mat background_;
    cv::Mat last_viz_;
    std::vector<MotionEvent> history_;
    bool initialized_ = false;
};

