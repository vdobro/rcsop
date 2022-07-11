#include "utils/chronometer.h"

#include <iostream>
#include <iomanip>

namespace rcsop::common::utils::time {
    using std::clog;
    using std::endl;
    using std::fixed;
    using std::setw;
    using std::setprecision;

    timer_seconds start_time() {
        return std::chrono::steady_clock::now();
    }

    double get_time_seconds(timer_seconds timer) {
        return (start_time() - timer).count();
    }

    timer_seconds log_and_start_next(timer_seconds last_timer,
                                     const string& message) {
        auto last_duration = get_time_seconds(last_timer);
        clog << "[ " << fixed << setw(7) << setprecision(3)
             << last_duration << " s ]: " << message << endl;

        return start_time();
    }
}