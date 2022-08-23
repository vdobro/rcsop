#ifndef RCSOP_COMMON_VECTOR_H
#define RCSOP_COMMON_VECTOR_H

#include <vector>
#include <functional>
#include <algorithm>
#include <execution>
#include <type_traits>
#include <ranges>

#include "utils/logging.h"

#define PARALLEL_EXECUTOR std::execution::par

namespace rcsop::common::utils {

    using std::vector;
    using std::function;

    template<typename T>
    vector<size_t> get_indices(const vector<T>& source) {
        vector<size_t> result;
        result.resize(source.size());
        for (size_t i = 0; i < source.size(); i++) {
            result[i] = i;
        }
        return result;
    }

    template<typename Source, typename Target, bool Parallel = false>
    vector<Target> map_vec(const vector<Source>& source,
                           const function<Target(const Source&)>& mapper) {
        vector<Target> result;
        if constexpr (std::is_default_constructible<Target>::value) {
            result.resize(source.size());
            if constexpr (Parallel) {
                std::transform(PARALLEL_EXECUTOR, source.begin(), source.end(), result.begin(), mapper);
            } else {
                std::transform(source.begin(), source.end(), result.begin(), mapper);
            }
            return result;
        }

        if constexpr (Parallel) {
            std::mutex vector_lock;
            std::for_each(PARALLEL_EXECUTOR,
                          source.cbegin(), source.cend(), [&vector_lock, &result, &mapper]
                                  (const Source& value) {
                        const auto mapped_value = mapper(value);

                        const std::lock_guard<std::mutex> lock(vector_lock);
                        result.push_back(mapped_value);
                    });
            return result;
        }

        for (const Source& value: source) {
            Target mapped_value = mapper(value);
            result.push_back(mapped_value);
        }
        return result;
    }

    template<typename Source, typename Target, bool Parallel = false>
    requires std::is_default_constructible<Target>::value
    shared_ptr<vector<Target>> map_vec_shared(const vector<Source>& source,
                                              const function<Target(const Source&)>& mapper) {
        auto result = make_shared<vector<Target>>();
        result->resize(source.size());

        if constexpr (Parallel) {
            std::transform(PARALLEL_EXECUTOR, source.cbegin(), source.cend(), result->begin(), mapper);
        } else {
            std::transform(source.cbegin(), source.cend(), result->begin(), mapper);
        }
        return result;
    }

    template<typename Source, typename Target, bool Parallel = false>
    vector<Target> map_vec(
            const vector<Source>& source,
            const function<Target(const size_t, const Source&)>& mapper) {
        const vector<size_t> indexes = get_indices(source);

        return map_vec<size_t, Target, Parallel>(indexes, [&source, &mapper](const size_t index) {
            const auto& value = source[index];
            return mapper(index, value);
        });
    }

    template<typename Source, typename Target>
    vector<Target> cast_vec(const vector<Source>& values) {
        return map_vec<Source, Target>(values, [](const Source value) {
            return static_cast<Target>(value);
        });
    }

    template<typename DataType>
    size_t find_nearest_index(DataType search_value,
                          const vector<DataType>& list) {
        auto iterator = std::min_element(list.cbegin(), list.cend(),
                                         [&search_value](const DataType& a, const DataType& b){
                                             return abs(search_value - a) < abs(search_value - b);
                                         });
        auto index = std::distance(list.cbegin(), iterator);
        return index;
    }

    template<typename DataType>
    DataType find_nearest(DataType search_value,
                          const vector<DataType>& list) {
        const size_t index = find_nearest_index(search_value, list);
        return list[index];
    }
}

#endif //RCSOP_COMMON_VECTOR_H
