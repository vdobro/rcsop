#ifndef RCSOP_COMMON_RANDOM_H
#define RCSOP_COMMON_RANDOM_H

#include <random>
#include <functional>

namespace rcsop::common::utils {
    using std::function;

    template<typename FloatingType = double>
    requires std::is_floating_point_v<FloatingType>
    auto get_uniform_distribution(FloatingType lower_bound,
                                  FloatingType upper_bound,
                                  bool upper_bound_exclusive)
    -> function<FloatingType(void)> {
        auto actual_upper_bound = upper_bound_exclusive ? upper_bound - STANDARD_ERROR : upper_bound;

        std::random_device rd;
        auto generator = make_shared<std::mt19937>(rd());
        auto distribution = make_shared<std::uniform_real_distribution<FloatingType>>(lower_bound, actual_upper_bound);
        return [generator, distribution]() -> FloatingType {
            return (*distribution)(*generator);
        };
    }

    template<typename FloatingType = double>
    auto get_uniform_distribution(FloatingType epsilon,
                                  bool upper_bound_exclusive)
    -> function<FloatingType(void)> {
        return get_uniform_distribution<FloatingType>(-epsilon, epsilon, upper_bound_exclusive);
    }
}

#endif //RCSOP_COMMON_RANDOM_H
