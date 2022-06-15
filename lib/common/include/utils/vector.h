#ifndef RCSOP_COMMON_VECTOR_H
#define RCSOP_COMMON_VECTOR_H

#include <vector>
#include <algorithm>
#include <type_traits>

using std::vector;
using std::function;

template<typename Source, typename Target>
vector<Target> map_vec(const vector<Source>& source,
                       const function<Target(const Source&)>& mapper) {
    vector<Target> result;
    if constexpr(std::is_default_constructible<Target>::value) {
        result.resize(source.size());
        std::transform(source.begin(), source.end(), result.begin(), mapper);
    } else {
        for (const auto & value : source) {
            result.push_back(mapper(value));
        }
    }
    return result;
}

template<typename Source, typename Target>
vector<Target> cast_vec(const vector<Source>& values) {
    return map_vec<Source, Target>(values, [](const Source value) {
        return static_cast<Target>(value);
    });
}

#endif //RCSOP_COMMON_VECTOR_H
