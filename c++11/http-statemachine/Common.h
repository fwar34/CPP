#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include <string>
#include <memory>

class Common
{
public:
    static void InitLog(spdlog::level::level_enum lvl, const std::string& logPath);
    static inline std::shared_ptr<spdlog::logger> Logger()
    {
        return logger_;
    }

private:
    static std::shared_ptr<spdlog::logger> logger_;
};