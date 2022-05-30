#include "utils/chronometer.h"

#include <iostream>
#include <iomanip>

timer_seconds start_time() {
    return std::chrono::steady_clock::now();
}

double get_time_seconds(timer_seconds timer) {
    return (start_time() - timer).count();
}

using std::clog;

timer_seconds log_and_start_next(timer_seconds last_timer, const std::string& message) {
    auto last_duration = get_time_seconds(last_timer);
    std::clog << "[ " << std::fixed << std::setw(7) << std::setprecision(3)
              << last_duration << " s ]: " << message << std::endl;

    return start_time();
}