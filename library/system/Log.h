#ifndef PROJECT_LOG_H
#define PROJECT_LOG_H

namespace library
{
#define note(fmt, ...) {printf(fmt, ##__VA_ARGS__);printf("\n");}
#define error(fmt, ...) {printf(fmt, ##__VA_ARGS__);printf("\n");}
}

#endif //PROJECT_LOG_H
