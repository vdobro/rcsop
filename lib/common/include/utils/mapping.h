#ifndef RCSOP_COMMON_VECTOR_H
#define RCSOP_COMMON_VECTOR_H

#include <vector>
#include <functional>
#include <algorithm>
#include <execution>
#include <type_traits>
#include <ranges>

#include "utils/logging.h"

#define PARALLEL_EXECUTOR std::execution::par_unseq

namespace rcsop::common::utils {

    using std::vector;
    using std::function;

    template<typename Source, typename Target, bool Parallel = false>
    vector<Target> map_vec(const vector<Source>& source,
                           const function<Target(const Source&)>& mapper) {
        vector<Target> result;
        if constexpr(std::is_default_constructible<Target>::value) {
            result.resize(source.size());
            if constexpr (Parallel) {
                std::transform(PARALLEL_EXECUTOR, source.begin(), source.end(), result.begin(), mapper);
            } else {
                std::transform(source.begin(), source.end(), result.begin(), mapper);
            }
            return result;
        }

        if constexpr(Parallel) {
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

        if constexpr(Parallel) {
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
        std::ranges::iota_view index_range(0ul, source.size());
        const vector<size_t> indexes(index_range.begin(), index_range.end());

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
}

#endif //RCSOP_COMMON_VECTOR_H
