#ifndef RCSOP_COMMON_LOGGING_H
#define RCSOP_COMMON_LOGGING_H

#include <string>

namespace rcsop::common::utils::logging {
    using std::string;

    template<bool condition>
    struct warn_if {
    };

    template<>
    struct [[deprecated]] warn_if<false> {
        constexpr warn_if() = default;
    };

    template<bool x>
    constexpr void static_warn() {
        warn_if<x>();
    }

    string construct_log_prefix(size_t current, size_t last);
}

#endif //RCSOP_COMMON_LOGGING_H
