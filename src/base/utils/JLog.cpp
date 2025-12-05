#include "base/utils/JLog.h"

namespace jr {

static std::shared_ptr<spdlog::logger> g_logger;

void JLog::Init(const std::string& name) {
    if (g_logger) return;

    g_logger = spdlog::stdout_color_mt(name);
    g_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%l][%t] %v");
    g_logger->set_level(spdlog::level::info);
}

void JLog::SetLevelDebug() { g_logger->set_level(spdlog::level::debug); }
void JLog::SetLevelInfo()  { g_logger->set_level(spdlog::level::info); }
void JLog::SetLevelWarn()  { g_logger->set_level(spdlog::level::warn); }
void JLog::SetLevelError() { g_logger->set_level(spdlog::level::err); }

std::shared_ptr<spdlog::logger>& JLog::GetLogger() {
    return g_logger;
}

} // namespace jr
