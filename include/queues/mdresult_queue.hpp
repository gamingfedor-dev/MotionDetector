#include "thread_queue.hpp"
#include "../models/detection_result.hpp"

class MotionDetectionResultQueue : public ThreadQueue<DetectionResult> {};