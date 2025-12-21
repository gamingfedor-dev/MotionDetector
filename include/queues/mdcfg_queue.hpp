#pragma once
#include "thread_queue.hpp"
#include "../models/mdcfg.hpp"

class MotionDetectionConfigQueue : public ThreadQueue<MotionDetectorConfig> {};