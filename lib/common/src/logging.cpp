#include "utils/logging.h"

#include <iomanip>
#include <cmath>
using std::stringstream;

string construct_log_prefix(size_t current, size_t last) {
    int width = ceil(log10(static_cast<double>(last)));
    std::stringstream ss;
    ss << "(" << std::setw(width) << std::to_string(current)
       << "/" << std::setw(width) << std::to_string(last)
       << ")\t ";
    return ss.str();
}