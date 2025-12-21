#include "main_window.hpp"

MainWindow::MainWindow(MotionDetectionConfigQueue& config_queue,
                       MotionDetectionResultQueue& result_queue,
                       QWidget* parent)
    : QMainWindow(parent)
    , config_queue_(config_queue)
    , result_queue_(result_queue)
{
    setupUI();
    
    // Timer to poll results queue
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer_->start(16);  // ~60 FPS
    
    // Send initial config
    sendConfig();
}

MainWindow::~MainWindow() {
    timer_->stop();
}

void MainWindow::setupUI() {
    setWindowTitle("Motion Detector");
    setMinimumSize(1000, 700);

    QWidget* central = new QWidget(this);
    QHBoxLayout* main_layout = new QHBoxLayout(central);

    // Left side - video display
    QVBoxLayout* video_layout = new QVBoxLayout();
    video_label_ = new QLabel();
    video_label_->setMinimumSize(640, 480);
    video_label_->setStyleSheet("background-color: black;");
    video_label_->setAlignment(Qt::AlignCenter);
    
    stats_label_ = new QLabel("Motion: 0% | Objects: 0");
    stats_label_->setStyleSheet("font-size: 14px; padding: 5px;");
    
    video_layout->addWidget(video_label_);
    video_layout->addWidget(stats_label_);

    // Right side - controls
    QGroupBox* controls_group = new QGroupBox("Controls");
    QVBoxLayout* controls_layout = new QVBoxLayout(controls_group);
    controls_group->setMaximumWidth(250);

    // Helper to create slider with label
    auto createSlider = [&](const QString& name, int min, int max, int value) {
        QLabel* label = new QLabel(QString("%1: %2").arg(name).arg(value));
        QSlider* slider = new QSlider(Qt::Horizontal);
        slider->setRange(min, max);
        slider->setValue(value);
        
        connect(slider, &QSlider::valueChanged, [=](int val) {
            label->setText(QString("%1: %2").arg(name).arg(val));
            onConfigChanged();
        });
        
        controls_layout->addWidget(label);
        controls_layout->addWidget(slider);
        return slider;
    };

    roi_x_slider_ = createSlider("ROI X", 0, 100, 50);
    roi_y_slider_ = createSlider("ROI Y", 0, 100, 60);
    roi_width_slider_ = createSlider("ROI Width", 5, 100, 25);
    roi_height_slider_ = createSlider("ROI Height", 5, 100, 60);
    threshold_slider_ = createSlider("Threshold", 1, 100, 25);
    min_area_slider_ = createSlider("Min Area (x100)", 1, 100, 1);

    controls_layout->addStretch();

    // Assemble layout
    main_layout->addLayout(video_layout, 1);
    main_layout->addWidget(controls_group);

    setCentralWidget(central);
}

void MainWindow::onConfigChanged() {
    sendConfig();
}

void MainWindow::sendConfig() {
    MotionDetectorConfig cfg;
    cfg.roi.center_x = roi_x_slider_->value() / 100.0f;
    cfg.roi.center_y = roi_y_slider_->value() / 100.0f;
    cfg.roi.width_ratio = std::max(0.05f, roi_width_slider_->value() / 100.0f);
    cfg.roi.height_ratio = std::max(0.05f, roi_height_slider_->value() / 100.0f);
    cfg.threshold = threshold_slider_->value();
    cfg.min_contour_area = min_area_slider_->value() * 100;
    
    config_queue_.push(std::move(cfg));
}

void MainWindow::updateFrame() {
    if (auto result = result_queue_.tryPopLatest()) {
        // Convert cv::Mat to QImage and display
        QImage img = cvMatToQImage(result->visualization);
        video_label_->setPixmap(QPixmap::fromImage(img).scaled(
            video_label_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        
        // Update stats
        stats_label_->setText(QString("Motion: %1% | Objects: %2 | Frame: %3")
            .arg(result->event.motion_score, 0, 'f', 2)
            .arg(result->event.contour_count)
            .arg(result->event.frame_id));
    }
}

QImage MainWindow::cvMatToQImage(const cv::Mat& mat) {
    if (mat.empty()) return QImage();
    
    cv::Mat rgb;
    cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    
    return QImage(rgb.data, rgb.cols, rgb.rows, 
                  rgb.step, QImage::Format_RGB888).copy();
}