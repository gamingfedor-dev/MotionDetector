#pragma once
#include <QObject>
#include <QImage>
#include <QTimer>
#include "mdcfg_queue.hpp"
#include "mdresult_queue.hpp"
#include "mdcfg.hpp"

class MotionComponent : public QObject {
    Q_OBJECT
    
    // Properties exposed to QML
    Q_PROPERTY(QImage frame READ frame NOTIFY frameChanged)
    Q_PROPERTY(double motionScore READ motionScore NOTIFY statsChanged)
    Q_PROPERTY(int objectCount READ objectCount NOTIFY statsChanged)
    Q_PROPERTY(int frameId READ frameId NOTIFY statsChanged)
    
    Q_PROPERTY(int roiX READ roiX WRITE setRoiX NOTIFY configChanged)
    Q_PROPERTY(int roiY READ roiY WRITE setRoiY NOTIFY configChanged)
    Q_PROPERTY(int roiWidth READ roiWidth WRITE setRoiWidth NOTIFY configChanged)
    Q_PROPERTY(int roiHeight READ roiHeight WRITE setRoiHeight NOTIFY configChanged)
    Q_PROPERTY(int threshold READ threshold WRITE setThreshold NOTIFY configChanged)
    Q_PROPERTY(int minArea READ minArea WRITE setMinArea NOTIFY configChanged)

public:
    explicit MotionComponent(MotionDetectionConfigQueue& config_queue,
                          MotionDetectionResultQueue& result_queue,
                          QObject* parent = nullptr);
    ~MotionComponent();

    // Getters
    QImage frame() const { return frame_; }
    double motionScore() const { return motion_score_; }
    int objectCount() const { return object_count_; }
    int frameId() const { return frame_id_; }
    
    int roiX() const { return roi_x_; }
    int roiY() const { return roi_y_; }
    int roiWidth() const { return roi_width_; }
    int roiHeight() const { return roi_height_; }
    int threshold() const { return threshold_; }
    int minArea() const { return min_area_; }

    // Setters
    void setRoiX(int value);
    void setRoiY(int value);
    void setRoiWidth(int value);
    void setRoiHeight(int value);
    void setThreshold(int value);
    void setMinArea(int value);
    void setConfigParam(int& param, int value);

signals:
    void frameChanged();
    void statsChanged();
    void configChanged();

private slots:
    void updateFrame();

private:
    void sendConfig();
    QImage cvMatToQImage(const cv::Mat& mat);

    MotionDetectionConfigQueue& config_queue_;
    MotionDetectionResultQueue& result_queue_;
    QTimer* timer_;

    // Current frame and stats
    QImage frame_;
    double motion_score_ = 0.0;
    int object_count_ = 0;
    int frame_id_ = 0;

    // Config values
    int roi_x_ = 50;
    int roi_y_ = 60;
    int roi_width_ = 25;
    int roi_height_ = 60;
    int threshold_ = 25;
    int min_area_ = 1;
};