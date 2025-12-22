# Motion Detector V4.0.5

## Architecture

### Video Capture Flow
1. VideoCapture reads video file
2. FrameQueue.push(frame)
3. MotionConsumer pops frame from FrameQueue
4. MotionConsumer pops config from ConfigQueue (if new)
5. MotionDetector.process(frame, config)
6. ResultQueue.push(result)
7. QTimer triggers every 16ms
8. MotionComponent::updateFrame()
9. ResultQueue.pop(result)
10. QImage displays frame

### Config Transfer Flow
1. User moves slider
2. MotionComponent::sendConfig()
3. ConfigQueue.push(config)
4. MotionConsumer picks it up on next loop iteration