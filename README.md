Video Capture flow:
  VideoCapture reads video file
        ↓
  FrameQueue.push(frame)
        ↓
  MotionConsumer pops frame from FrameQueue
        ↓
  MotionConsumer pops config from ConfigQueue (if any new)
        ↓
  MotionDetector.process(frame, config)
        ↓
  ResultQueue.push(result)
        ↓
  QTimer triggers every 16ms
        ↓
  MainWindow::updateFrame()
        ↓
  ResultQueue.pop(result)
        ↓
  QLabel displays frame




Message (config) transfer flow:
  User moves slider
        ↓
  MainWindow::sendConfig()
        ↓
  ConfigQueue.push(config)
        ↓
  MotionConsumer picks it up on next loop iteration