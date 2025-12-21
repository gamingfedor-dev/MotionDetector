#include <QApplication>
#include "frame_queue.hpp"
#include "video_capture.hpp"
#include "motion_consumer.hpp"
#include "main_window.hpp"
#include <iostream>
#include <csignal>

std::atomic<bool> g_running{true};
void signalHandler(int) { g_running = false; QApplication::quit();}

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    std::signal(SIGINT, signalHandler);
    
    std::string source = argc > 1 ? argv[1] : "0";
    std::string output = argc > 2 ? argv[2] : "motion_data.csv";

    std::cout << "Motion Detector - Qt GUI\n";
    
    std::cout << "Motion Detector - Press Q to quit\n";
    
    MotionDetectionConfigQueue config_queue;
    MotionDetectionResultQueue result_queue;
    FrameQueue frame_queue(30);

    VideoCapture capture(source, frame_queue);
    MotionConsumer consumer(frame_queue, config_queue, result_queue);
    
    if (!capture.start()) {
        std::cerr << "Failed to start capture" << std::endl;
        return 1;
    }
    
    consumer.start();

    MainWindow window(config_queue, result_queue);
    window.show();
    
    int result = app.exec();
    
    consumer.stop();
    capture.stop();
    consumer.exportCSV(output);
    
    std::cout << "Data exported to " << output << std::endl;
    return result;
}