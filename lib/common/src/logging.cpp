#include "utils/logging.h"

#include <iomanip>
#include <cmath>

namespace rcsop::common::utils::logging {
    using std::stringstream;
    using std::setw;
    using std::to_string;

    string construct_log_prefix(size_t current, size_t last) {
        int width = ceil(log10(static_cast<double>(last)));
        stringstream ss;

        ss << "(" << setw(width) << to_string(current)
           << "/" << setw(width) << to_string(last)
           << ")\t ";
        return ss.str();
    }
}
