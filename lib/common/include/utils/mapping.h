#ifndef RCSOP_COMMON_VECTOR_H
#define RCSOP_COMMON_VECTOR_H

#include <vector>
#include <functional>
#include <algorithm>
#include <execution>
#include <type_traits>
#include <ranges>

using std::vector;
using std::function;

template<typename Source, typename Target, bool Parallel = true>
vector<Target> map_vec(const vector<Source>& source,
                       const function<Target(const Source&)>& mapper) {
    vector<Target> result;
    if constexpr(std::is_default_constructible<Target>::value) {
        result.resize(source.size());
        if constexpr (Parallel) {
            std::transform(std::execution::par_unseq, source.begin(), source.end(), result.begin(), mapper);
        } else {
            std::transform(source.begin(), source.end(), result.begin(), mapper);
        }
    } else {
        static_assert(!Parallel, "Cannot parallelize for target types without a default constructor");
        for (const auto& value: source) {
            result.push_back(mapper(value));
        }
    }
    return result;
}

template<typename Source, typename Target, bool Parallel = true>
shared_ptr<vector<Target>> map_vec_shared(const vector<Source>& source,
                                          const function<Target(const Source&)>& mapper) {
    static_assert(std::is_default_constructible<Target>::value);
    auto result = make_shared<vector<Target>>();
    result->resize(source.size());

    if constexpr(Parallel) {
        std::transform(std::execution::par_unseq, source.begin(), source.end(), result->begin(), mapper);
    } else {
        std::transform(source.begin(), source.end(), result->begin(), mapper);
    }
    return result;
}

/*
template<typename Source, typename Target>
shared_ptr<vector<Target>> map_vec_shared(const vector<Source>& source,
                                   const function<Target(const size_t, const Source&)>& mapper) {
    static_assert(std::is_default_constructible<Target>::value);
    auto result = make_shared<vector<Target>>();
    std::ranges::iota_view indexes(0ul, source.size());
    result.resize(source.size());

    std::for_each(
            std::execution::par_unseq,
            indexes.begin(), indexes.end(),
            [&result, &source, &mapper](const size_t index) {
                const auto& value = source[index];
                (*result)[index] = mapper(index, value);
            });
    return result;
}
 */

template<typename Source, typename Target>
vector<Target> cast_vec(const vector<Source>& values) {
    return map_vec<Source, Target>(values, [](const Source value) {
        return static_cast<Target>(value);
    });
}


template<typename InputValueType, typename DataType>
size_t find_interval_match_index(InputValueType search_value,
                                 DataType first_range,
                                 DataType range_epsilon) {
    return lround((static_cast<DataType>(search_value) - first_range) / (2 * range_epsilon));
}

template<typename InputValueType, typename DataType, typename MappedValueType>
MappedValueType find_interval_match(InputValueType search_value,
                                    const vector<MappedValueType>& mapped_values,
                                    DataType first_range,
                                    DataType range_epsilon) {
    size_t index = find_interval_match_index(search_value, first_range, range_epsilon);
    if (index >= mapped_values.size()) {
        return 0;
    }
    return mapped_values.at(index);
}

#endif //RCSOP_COMMON_VECTOR_H
