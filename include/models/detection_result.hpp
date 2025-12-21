#pragma once
#include <opencv2/opencv.hpp>
#include "motion_event.hpp"

struct DetectionResult {
    cv::Mat visualization;
    MotionEvent event;
};