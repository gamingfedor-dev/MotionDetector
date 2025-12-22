#include <QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include "frame_queue.hpp"
#include "video_capture.hpp"
#include "motion_consumer.hpp"
#include <iostream>
#include <csignal>
#include <motion_component.hpp>
#include <frame_image_service.hpp>

std::atomic<bool> g_running{true};
void signalHandler(int) { g_running = false; QGuiApplication::quit();}

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
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

    MotionComponent component(config_queue, result_queue);
    
    /** Heap instantiation, Qt manages memory itself */
    FrameImageService* imageService = new FrameImageService();
    /** Grab processed frame converted to QImage from component once QML is requesting image */
    imageService->setFrameCallback([&component]() {
        return component.frame();
    });

    QQmlApplicationEngine engine;
    
    engine.addImageProvider("frame", imageService);
    engine.rootContext()->setContextProperty("component", &component);
    engine.load(QUrl("qrc:/qml/Main.qml"));
    
    if (engine.rootObjects().isEmpty()) {
        std::cerr << "Failed to load QML" << std::endl;
        return 1;
    }
    
    int result = app.exec();
    
    consumer.stop();
    capture.stop();
    consumer.exportCSV(output);
    
    std::cout << "Data exported to " << output << std::endl;
    return result;
}