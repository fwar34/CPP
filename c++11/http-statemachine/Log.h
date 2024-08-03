#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/logger.h>
#include <string>
#include <memory>

class Log
{
public:
    static void InitLog(spdlog::level::level_enum lvl);
    static inline std::shared_ptr<spdlog::logger> Logger()
    {
        return logger_;
    }

private:
    static std::shared_ptr<spdlog::logger> logger_;
};

// 内联函数放到头文件定义，在其他文件包含的时候可以直接替换，否则可能导致链接问题
inline void Log::InitLog(spdlog::level::level_enum lvl)
{
    logger_->set_level(lvl);
}