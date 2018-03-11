#ifndef PROJECT_LOG_H
#define PROJECT_LOG_H

#include <spdlog/spdlog.h>

namespace library
{

static std::shared_ptr<spdlog::logger> logger = spdlog::stderr_logger_mt("console");
//static std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("Log", "log.txt");

#define note(...) library::logger->info(__VA_ARGS__)
#define error(...) library::logger->error(__VA_ARGS__)

//#define note(fmt, ...) {printf(fmt, ##__VA_ARGS__);printf("\n");}
//#define error(fmt, ...) {printf(fmt, ##__VA_ARGS__);printf("\n");}
}

#endif //PROJECT_LOG_H
