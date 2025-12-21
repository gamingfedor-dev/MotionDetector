#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <opencv2/opencv.hpp>
#include <QTimer>
#include "mdcfg_queue.hpp"
#include "mdresult_queue.hpp"
#include "mdcfg.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(MotionDetectionConfigQueue& config_queue, 
                        MotionDetectionResultQueue& result_queue,
                        QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void updateFrame();
    void onConfigChanged();

private:
    QImage cvMatToQImage(const cv::Mat& mat);
    void setupUI();
    void sendConfig();

    // UI elements
    QLabel* video_label_;
    QLabel* stats_label_;
    
    QSlider* roi_x_slider_;
    QSlider* roi_y_slider_;
    QSlider* roi_width_slider_;
    QSlider* roi_height_slider_;
    QSlider* threshold_slider_;
    QSlider* min_area_slider_;

    // Queues
    MotionDetectionConfigQueue& config_queue_;
    MotionDetectionResultQueue& result_queue_;

    // Update timer
    QTimer* timer_;
};