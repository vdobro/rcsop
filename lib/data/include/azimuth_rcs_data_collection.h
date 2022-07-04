#ifndef RCSOP_DATA_AZIMUTH_RCS_MAP_H
#define RCSOP_DATA_AZIMUTH_RCS_MAP_H

#include "abstract_rcs_map.h"
#include "az_data.h"

class AzimuthRcsDataCollection : public AbstractDataCollection {
private:
    shared_ptr<map<height_t, map<azimuth_t, shared_ptr<AzimuthRcsDataSet>>>> _data;
public:
    explicit AzimuthRcsDataCollection(const path& input_path);

    [[nodiscard]] shared_ptr<AbstractDataSet> get_for_exact_position(const Observer& observer) const override;

    void use_filtered_peaks();
};

#endif //RCSOP_DATA_AZIMUTH_RCS_MAP_H