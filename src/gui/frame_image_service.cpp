#include "frame_image_service.hpp"

FrameImageService::FrameImageService()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage FrameImageService::requestImage(const QString& id, QSize* size, const QSize& requestedSize) {
    Q_UNUSED(id)
    Q_UNUSED(requestedSize)
    
    QImage frame;
    if (frame_callback_) {
        frame = frame_callback_();
    }
    
    if (frame.isNull()) {
        frame = QImage(640, 480, QImage::Format_RGB888);
        frame.fill(Qt::black);
    }
    
    if (size) {
        *size = frame.size();
    }
    
    return frame;
}

void FrameImageService::setFrameCallback(std::function<QImage()> callback) {
    frame_callback_ = callback;
}