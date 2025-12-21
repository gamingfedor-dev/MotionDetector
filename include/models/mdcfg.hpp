#pragma once
#include "roi_config.hpp"

struct MotionDetectorConfig {
    ROIConfig roi;
    int threshold = 25;
    int blur_kernel = 21;
    int min_contour_area = 100;
    double learning_rate = 0.01;
    bool reset_background = false;
};