#pragma once

#include <spdlog/spdlog.h>

namespace library
{
struct LogContext
{
    LogContext(const std::shared_ptr<spdlog::logger> &logger);
    static std::shared_ptr<spdlog::logger> getLogger();

private:
    static std::shared_ptr<spdlog::logger> logger;
};

#define note(...) LogContext::getLogger()->info(__VA_ARGS__)
#define error(...) LogContext::getLogger()->error(__VA_ARGS__)
}

