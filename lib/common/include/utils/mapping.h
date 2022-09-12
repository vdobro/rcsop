#ifndef RCSOP_COMMON_VECTOR_H
#define RCSOP_COMMON_VECTOR_H

#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <execution>
#include <type_traits>
#include <ranges>

#include "utils/logging.h"

constexpr auto PARALLEL = std::execution::par;
constexpr auto PARALLEL_VECTORIZED = std::execution::par_unseq;

constexpr bool DEFAULT_PARALLEL_ENABLED = true;
constexpr bool DEFAULT_VECTORIZED_ENABLED = true;

namespace rcsop::common::utils {
    using std::vector;
    using std::function;
    using std::shared_ptr;
    using std::min_element;
    using std::max_element;
    using std::copy_if;
    using std::back_inserter;
    using std::distance;

    template<typename T>
    vector<size_t> get_indices(const vector<T>& source) {
        vector<size_t> result;
        result.resize(source.size());
        for (size_t i = 0; i < source.size(); i++) {
            result[i] = i;
        }
        return result;
    }

    template<typename Source, typename Target,
            bool Parallel = DEFAULT_PARALLEL_ENABLED,
            bool Vectorized = DEFAULT_VECTORIZED_ENABLED>
    vector<Target> map_vec(const vector<Source>& source,
                           const function<Target(const Source&)>& mapper) {
        vector<Target> result;
        if constexpr (std::is_default_constructible<Target>::value) {
            result.resize(source.size());
            if constexpr (Parallel) {
                if constexpr (Vectorized) {
                    std::transform(PARALLEL_VECTORIZED, source.cbegin(), source.cend(), result.begin(), mapper);
                } else {
                    std::transform(PARALLEL, source.cbegin(), source.cend(), result.begin(), mapper);
                }
            } else {
                std::transform(source.cbegin(), source.cend(), result.begin(), mapper);
            }
            return result;
        }

        if constexpr (Parallel) {
            std::mutex vector_lock;
            auto value_mapper = [&vector_lock, &result, &mapper] (const Source& value) {
                const auto mapped_value = mapper(value);

                const std::lock_guard<std::mutex> lock(vector_lock);
                result.push_back(mapped_value);
            };
            if constexpr (Vectorized) {
                std::for_each(PARALLEL_VECTORIZED,
                              source.cbegin(), source.cend(),
                              value_mapper);
            } else {
                std::for_each(PARALLEL,
                              source.cbegin(), source.cend(),
                              value_mapper);
            }
            return result;
        }

        for (const Source& value: source) {
            Target mapped_value = mapper(value);
            result.push_back(mapped_value);
        }
        return result;
    }

    template<typename Source, typename Target,
            bool Parallel = DEFAULT_PARALLEL_ENABLED,
            bool Vectorized = DEFAULT_VECTORIZED_ENABLED>
    requires std::is_default_constructible<Target>::value
    shared_ptr<vector<Target>> map_vec_shared(const vector<Source>& source,
                                              const function<Target(const Source&)>& mapper) {
        auto result = make_shared<vector<Target>>();
        result->resize(source.size());

        if constexpr (Parallel) {
            if constexpr (Vectorized) {
                std::transform(PARALLEL_VECTORIZED, source.cbegin(), source.cend(), result->begin(), mapper);
            } else {
                std::transform(PARALLEL, source.cbegin(), source.cend(), result->begin(), mapper);
            }
        } else {
            std::transform(source.cbegin(), source.cend(), result->begin(), mapper);
        }
        return result;
    }

    template<typename Source, typename Target,
            bool Parallel = DEFAULT_PARALLEL_ENABLED,
            bool Vectorized = DEFAULT_VECTORIZED_ENABLED>
    vector<Target> map_vec(
            const vector<Source>& source,
            const function<Target(const size_t, const Source&)>& mapper) {
        const vector<size_t> indexes = get_indices(source);

        return map_vec<size_t, Target, Parallel, Vectorized>(indexes, [&source, &mapper](const size_t index) {
            const auto& value = source[index];
            return mapper(index, value);
        });
    }

    template<typename Value, class Comparable,
            bool Parallel = DEFAULT_PARALLEL_ENABLED,
            bool Vectorized = DEFAULT_VECTORIZED_ENABLED>
    void sort_in_place(vector<Value>& values,
                       std::function<Comparable(const Value&)> mapper) {
        auto comparator = [&mapper](const Value& a, const Value& b) {
            return mapper(a) < mapper(b);
        };
        if constexpr (Parallel) {
            if constexpr (Vectorized) {
                std::sort(PARALLEL_VECTORIZED,
                          values.begin(), values.end(), comparator);
            } else {
                std::sort(PARALLEL,
                          values.begin(), values.end(), comparator);
            }
        } else {
            std::sort(std::execution::unseq,
                      values.begin(), values.end(), comparator);
        }
    }

    template<typename Source, typename Target,
            bool Parallel = DEFAULT_PARALLEL_ENABLED,
            bool Vectorized = DEFAULT_VECTORIZED_ENABLED>
    vector<Target> cast_vec(const vector<Source>& values) {
        return map_vec<Source, Target, Parallel, Vectorized>(values, [](const Source value) {
            return static_cast<Target>(value);
        });
    }

    template<typename DataType>
    size_t find_nearest_index(DataType search_value,
                              const vector<DataType>& sorted_list) {
        const auto last = sorted_list.size() - 1;
        if (search_value < sorted_list[0]) {
            return 0;
        }
        if (search_value > sorted_list[last]) {
            return last;
        }

        size_t low = 0;
        size_t high = last;
        while (low <= high) {
            auto middle = (high + low) / 2;
            auto mid_value = sorted_list[middle];
            if (search_value < mid_value) {
                high = middle - 1;
            } else if (search_value > mid_value) {
                low = middle + 1;
            } else {
                return middle;
            }
        }
        return ((sorted_list[low] - search_value) < (search_value - sorted_list[high]))
                ? low : high;
    }

    template<typename DataType>
    DataType find_nearest(DataType search_value,
                          const vector<DataType>& sorted_list) {
        const size_t index = find_nearest_index(search_value, sorted_list);
        return sorted_list[index];
    }

    template<typename DataType>
    using predicate_func = function<bool(const DataType&)>;

    template<typename DataType>
    vector<DataType> filter_vec(const vector<DataType>& values,
                                const predicate_func<DataType>& filter_predicate) {
        vector<DataType> filtered_values;
        copy_if(values.cbegin(), values.cend(), back_inserter(filtered_values), filter_predicate);
        return filtered_values;
    }

    template<typename DataType>
    shared_ptr<vector<DataType>> filter_vec_shared(
            const vector<DataType>& values,
            const predicate_func<DataType>& filter_predicate) {
        auto filtered_values = make_shared<vector<DataType>>();
        copy_if(values.cbegin(), values.cend(), back_inserter(*filtered_values), filter_predicate);
        return filtered_values;
    }

    template<typename ValueType>
    auto min_value(const vector<ValueType>& values) -> ValueType {
        return *min_element(
                values.cbegin(), values.cend());
    }

    template<typename ValueType>
    auto max_value(const vector<ValueType>& values) -> ValueType {
        return *max_element(
                values.cbegin(), values.cend());
    }
}

#endif //RCSOP_COMMON_VECTOR_H
