#pragma once
#include "thread_queue.hpp"
#include "mdcfg.hpp"

class MotionDetectionConfigQueue : public ThreadQueue<MotionDetectorConfig> {};