#ifndef RCSOP_DATA_AZIMUTH_RCS_MAP_H
#define RCSOP_DATA_AZIMUTH_RCS_MAP_H

#include "az_data.h"

class AzimuthRcsMap {
private:
    shared_ptr<map<height_t, map<azimuth_t, shared_ptr<AzimuthRcsDataSet>>>> _data;
public:
    explicit AzimuthRcsMap(const path& input_path);

    [[nodiscard]] vector<height_t> heights() const;
    [[nodiscard]] map<azimuth_t, shared_ptr<AzimuthRcsDataSet>> at_height(height_t height) const;
    [[nodiscard]] shared_ptr<AzimuthRcsDataSet> at_position(const ObserverPosition& position) const;
};

#endif //RCSOP_DATA_AZIMUTH_RCS_MAP_H
