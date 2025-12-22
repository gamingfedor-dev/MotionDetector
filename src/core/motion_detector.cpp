#include "motion_detector.hpp"
#include <algorithm>
#include "motion_event.hpp"

MotionDetector::MotionDetector(): MotionDetector(Config{}) {}
MotionDetector::MotionDetector(const Config& cfg) : config_(cfg) {}

void MotionDetector::setROI(const ROIConfig& roi) {
    config_.roi = roi;
    initialized_ = false;
}

MotionEvent MotionDetector::process(const cv::Mat& frame, uint64_t id, int64_t ts) {
    cv::Rect roi_rect = config_.roi.toRect(frame.cols, frame.rows);

    MotionEvent event{id, ts, 0.0, 0, cv::Rect(), roi_rect};

    cv::Mat roi_frame = frame(roi_rect);

    cv::Mat gray, blurred;
    cv::cvtColor(roi_frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(config_.blur_kernel, config_.blur_kernel), 0);

    if (!initialized_ || 
        background_.rows != blurred.rows || 
        background_.cols != blurred.cols) {
        blurred.convertTo(background_, CV_32F);
        initialized_ = true;
        last_viz_ = frame.clone();
        if (config_.draw_roi) {
            cv::rectangle(last_viz_, roi_rect, cv::Scalar(255, 0, 0), 2);
        }
        return event;
    }
    
    cv::Mat bg_8u, diff, thresh;
    background_.convertTo(bg_8u, CV_8U);
    cv::absdiff(blurred, bg_8u, diff);
    cv::threshold(diff, thresh, config_.threshold, 255, cv::THRESH_BINARY);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::dilate(thresh, thresh, kernel, cv::Point(-1,-1), 2);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    last_viz_ = frame.clone();
    double max_area = 0, total_area = 0;
    
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area < config_.min_contour_area) continue;
        
        event.contour_count++;
        total_area += area;

        // cv::Rect bbox = cv::boundingRect(contour);
        // bbox.x += roi_rect.x;
        // bbox.y += roi_rect.y;
        // cv::rectangle(last_viz_, bbox, cv::Scalar(0, 255, 0), 2);

        std::vector<cv::Point> adjusted_contour = contour;

        for (auto& point : adjusted_contour) {
            point.x += roi_rect.x;
            point.y += roi_rect.y;
        }
        
        cv::polylines(last_viz_, {adjusted_contour}, 0, cv::Scalar(0, 255, 0), 2);
        
        
        if (area > max_area) {
            max_area = area;
            // event.largest_bbox = bbox;
        }
    }

    if (config_.draw_roi) {
        cv::rectangle(last_viz_, roi_rect, cv::Scalar(255, 0, 0), 2);
        cv::putText(last_viz_, "ROI", cv::Point(roi_rect.x + 5, roi_rect.y + 20),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);
    }

    double roi_area = roi_rect.width * roi_rect.height;
    event.motion_score = (total_area / (frame.rows * frame.cols)) * 100.0;
    
    cv::Mat blurred_f;
    blurred.convertTo(blurred_f, CV_32F);
    cv::accumulateWeighted(blurred_f, background_, config_.learning_rate);
    
    history_.push_back(event);
    return event;
}
cv::Mat MotionDetector::getVisualization() const { return last_viz_; }

void MotionDetector::setConfig(const Config& cfg) {
    bool roi_changed = (config_.roi.center_x != cfg.roi.center_x ||
                        config_.roi.center_y != cfg.roi.center_y ||
                        config_.roi.width_ratio != cfg.roi.width_ratio ||
                        config_.roi.height_ratio != cfg.roi.height_ratio);
    
    config_ = cfg;
    
    if (roi_changed) {
        resetBackground();
    }
}

void MotionDetector::resetBackground() {
    initialized_ = false;
    background_.release();
}

void MotionDetector::exportCSV(const std::string& path) const {
    std::ofstream file(path);
    file << "frame_id,timestamp_ms,motion_score,contour_count\n";
    for (const auto& e : history_) {
        file << e.frame_id << "," << e.timestamp_ms << ","
             << e.motion_score << "," << e.contour_count << ","
             << e.roi_used.x << "," << e.roi_used.y << ","
             << e.roi_used.width << "," << e.roi_used.height << "\n";
    }
}
