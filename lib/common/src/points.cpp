#include "utils/points.h"

namespace rcsop::common::utils::points {

    auto find_farthest(const vec3& reference_point,
                       const vector<vec3>& search_values) -> vec3 {
        auto iterator = std::max_element(
                search_values.cbegin(), search_values.cend(),
                [&reference_point](const vec3& a, const vec3& b) {
                    return (reference_point - a).norm() < (reference_point - b).norm();
                });
        auto index = std::distance(search_values.cbegin(), iterator);
        return search_values[index];
    }
}