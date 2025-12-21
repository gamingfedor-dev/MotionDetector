#pragma once
#include <opencv2/opencv.hpp>

struct ROIConfig {
    float center_x = 0.5f;      // Center position (0.0 - 1.0)
    float center_y = 0.6f;
    float width_ratio = 0.3f;   // ROI is 30% of frame width
    float height_ratio = 0.8f;  // ROI is 50% of frame height
    
    cv::Rect toRect(int frame_width, int frame_height) const {
        int roi_w = static_cast<int>(frame_width * width_ratio);
        int roi_h = static_cast<int>(frame_height * height_ratio);
        int roi_x = static_cast<int>(frame_width * center_x - roi_w / 2);
        int roi_y = static_cast<int>(frame_height * center_y - roi_h / 2);
        
        // Clamp to frame bounds
        roi_x = std::max(0, std::min(roi_x, frame_width - roi_w));
        roi_y = std::max(0, std::min(roi_y, frame_height - roi_h));
        
        return cv::Rect(roi_x, roi_y, roi_w, roi_h);
    }
};