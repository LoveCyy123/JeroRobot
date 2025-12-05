#pragma once

#include <string>
#include <memory>

// 当前用 spdlog 做底层实现
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace jr {

class JLog {
public:
    static void Init(const std::string& name = "jero_robot");

    static void SetLevelDebug();
    static void SetLevelInfo();
    static void SetLevelWarn();
    static void SetLevelError();

    static std::shared_ptr<spdlog::logger>& GetLogger();
};

// 一些方便的宏，业务里全用这些
#define LOG_DEBUG(fmt, ...) ::jr::JLog::GetLogger()->debug(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  ::jr::JLog::GetLogger()->info(fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  ::jr::JLog::GetLogger()->warn(fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) ::jr::JLog::GetLogger()->error(fmt, ##__VA_ARGS__)

} // namespace jr
