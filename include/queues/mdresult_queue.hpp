#include "thread_queue.hpp"
#include "detection_result.hpp"

class MotionDetectionResultQueue : public ThreadQueue<DetectionResult> {};