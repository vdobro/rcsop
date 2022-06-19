#ifndef RCSOP_COMMON_VECTOR_H
#define RCSOP_COMMON_VECTOR_H

#include <vector>
#include <functional>
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
        for (const auto& value: source) {
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


template<typename InputValueType, typename SearchValue>
size_t find_interval_match_index(InputValueType search_value,
                                 SearchValue first_range,
                                 SearchValue range_epsilon) {
    return lround((static_cast<SearchValue>(search_value) - first_range) / (2 * range_epsilon));
}

template<typename InputValueType, typename SearchValue, typename MappedValue>
MappedValue find_interval_match(InputValueType search_value,
                                const vector<MappedValue>& mapped_values,
                                SearchValue first_range,
                                SearchValue range_epsilon) {
    size_t index = find_interval_match_index(search_value, first_range, range_epsilon);
    return mapped_values.at(index);
}


#endif //RCSOP_COMMON_VECTOR_H
