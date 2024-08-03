#include "Log.h"

std::shared_ptr<spdlog::logger> Log::logger_ = spdlog::basic_logger_mt("basic_logger", "log");