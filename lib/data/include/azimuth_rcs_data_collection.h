#ifndef RCSOP_DATA_AZIMUTH_RCS_MAP_H
#define RCSOP_DATA_AZIMUTH_RCS_MAP_H

#include "abstract_rcs_map.h"
#include "az_data.h"

class AzimuthRcsDataCollection : public AbstractDataCollection {
private:
    shared_ptr<map<height_t, map<azimuth_t, shared_ptr<AzimuthRcsDataSet>>>> _data;
public:
    explicit AzimuthRcsDataCollection(const path& input_path);

    [[nodiscard]] shared_ptr<AbstractDataSet> at_position(const ObserverPosition& position) const override;
};

#endif //RCSOP_DATA_AZIMUTH_RCS_MAP_H
