#include "motion_component.hpp"
#include <opencv2/opencv.hpp>

MotionComponent::MotionComponent(MotionDetectionConfigQueue& config_queue,
                             MotionDetectionResultQueue& result_queue,
                             QObject* parent)
    : QObject(parent)
    , config_queue_(config_queue)
    , result_queue_(result_queue)
{
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MotionComponent::updateFrame);
    timer_->start(16);
    
    sendConfig();
}

MotionComponent::~MotionComponent() {
    timer_->stop();
}

void MotionComponent::updateFrame() {
    if (auto result = result_queue_.tryPopLatest()) {
        frame_ = cvMatToQImage(result->visualization);
        motion_score_ = result->event.motion_score;
        object_count_ = result->event.contour_count;
        frame_id_ = result->event.frame_id;
        
        emit frameChanged();
        emit statsChanged();
    }
}

void MotionComponent::sendConfig() {
    MotionDetectorConfig cfg;
    cfg.roi.center_x = roi_x_ / 100.0f;
    cfg.roi.center_y = roi_y_ / 100.0f;
    cfg.roi.width_ratio = std::max(0.05f, roi_width_ / 100.0f);
    cfg.roi.height_ratio = std::max(0.05f, roi_height_ / 100.0f);
    cfg.threshold = threshold_;
    cfg.min_contour_area = min_area_ * 100;

    config_queue_.push(std::move(cfg));
}

void MotionComponent::setConfigParam(int& param, int value) {
    if (param != value) {
        param = value;
        emit configChanged();
        sendConfig();
    }
}

void MotionComponent::setRoiX(int value) { setConfigParam(roi_x_, value); }
void MotionComponent::setRoiY(int value) { setConfigParam(roi_y_, value); }
void MotionComponent::setRoiWidth(int value) { setConfigParam(roi_width_, value); }
void MotionComponent::setRoiHeight(int value) { setConfigParam(roi_height_, value); }
void MotionComponent::setThreshold(int value) { setConfigParam(threshold_, value); }
void MotionComponent::setMinArea(int value) { setConfigParam(min_area_, value); }

QImage MotionComponent::cvMatToQImage(const cv::Mat& mat) {
    if (mat.empty()) return QImage();
    
    cv::Mat rgb;
    cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    
    return QImage(rgb.data, rgb.cols, rgb.rows,
                  rgb.step, QImage::Format_RGB888).copy();
}