#include "Common.h"

inline void Common::InitLog(spdlog::level lvl, const std::string& logPath)
{
    logger_ = spdlog::basic_logger_mt("basic_logger", logPath);
    logger_->set_level(lvl);
}