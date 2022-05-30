#ifndef SFM_COLORING_COMMON_VECTOR_H
#define SFM_COLORING_COMMON_VECTOR_H

#include <vector>
#include <algorithm>
using std::vector;

template<typename Source, typename Target, typename MapFunction>
vector<Target> map_vec(const vector<Source>& vec,
                       MapFunction map_function) {
    vector<Target> result;
    result.resize(vec.size());
    std::transform(vec.begin(), vec.end(), result.begin(), map_function);
    return result;
}

#endif //SFM_COLORING_COMMON_VECTOR_H
