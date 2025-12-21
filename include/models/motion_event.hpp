#pragma once
#include <opencv2/opencv.hpp>

struct MotionEvent {
    uint64_t frame_id;
    int64_t timestamp_ms;
    double motion_score;
    int contour_count;
    cv::Rect largest_bbox;
    cv::Rect roi_used;
};
