#ifndef RCS_OVERLAY_PLOTTER_AZIMUTH_RCS_MAP_H
#define RCS_OVERLAY_PLOTTER_AZIMUTH_RCS_MAP_H

#include "az_data.h"

typedef long height_t;
typedef long azimuth_t;

typedef map<height_t, map<azimuth_t, shared_ptr<az_data>>> azimuth_map_t;

class AzimuthRcsMap {
private:
    shared_ptr<azimuth_map_t> data;
public:
    explicit AzimuthRcsMap(const path& input_path);

    vector<height_t> heights() const;
    shared_ptr<az_data> atHeightAndAngle(height_t height, azimuth_t angle) const;
};

#endif //RCS_OVERLAY_PLOTTER_AZIMUTH_RCS_MAP_H
