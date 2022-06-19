#ifndef RCSOP_COMMON_CHRONOMETER_H
#define RCSOP_COMMON_CHRONOMETER_H

#include <chrono>
#include <string>
using std::string;

typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>> timer_seconds;
timer_seconds start_time();

double get_time_seconds(timer_seconds timer);

timer_seconds log_and_start_next(timer_seconds last_timer, const string& message);

#endif //RCSOP_COMMON_CHRONOMETER_H
