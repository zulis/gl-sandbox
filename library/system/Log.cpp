#include "Log.h"

namespace library
{

std::shared_ptr<spdlog::logger> LogContext::logger = nullptr;

std::shared_ptr<spdlog::logger> LogContext::getLogger()
{
    if (!logger)
#ifdef _WIN32
        logger = spdlog::stderr_logger_mt("console");
#else
        logger = spdlog::stdout_color_mt("console");
#endif
    //static std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("Log", "log.txt");*/

    return logger;
}
}