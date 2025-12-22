#pragma once
#include <QtQuick/QQuickImageProvider>
#include <QImage>
#include <functional>

class FrameImageService : public QQuickImageProvider {
public:
    FrameImageService();
    
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
    
    void setFrameCallback(std::function<QImage()> callback);

private:
    std::function<QImage()> frame_callback_;
};