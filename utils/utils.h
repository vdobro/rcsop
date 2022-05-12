#pragma once

#include <algorithm>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>

using std::vector;

template<typename Source, typename Target, typename MapFunction>
vector<Target> map_vec(const vector<Source>& vec,
                       MapFunction map_function) {
    vector<Target> result;
    result.resize(vec.size());
    std::transform(vec.begin(), vec.end(), result.begin(), map_function);
    return result;
}

typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>> timer_seconds;
timer_seconds start_time();

double get_time_seconds(timer_seconds timer);

timer_seconds log_and_start_next(timer_seconds last_timer, const std::string& message);