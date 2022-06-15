#ifndef RCSO_DATA_AZIMUTH_RCS_MAP_H
#define RCSO_DATA_AZIMUTH_RCS_MAP_H

#include "az_data.h"

typedef long height_t;
typedef long azimuth_t;

typedef map<height_t, map<azimuth_t, shared_ptr<az_data>>> azimuth_map_t;

class AzimuthRcsMap {
private:
    shared_ptr<azimuth_map_t> data;
public:
    explicit AzimuthRcsMap(const path& input_path);

    [[nodiscard]] vector<height_t> heights() const;
    [[nodiscard]] map<azimuth_t, shared_ptr<az_data>> at_height(height_t height) const;
    [[nodiscard]] shared_ptr<az_data> at_position(const ObserverPosition& position) const;
};

#endif //RCSO_DATA_AZIMUTH_RCS_MAP_H
