#ifndef RCSOP_DATA_AZIMUTH_RCS_MAP_H
#define RCSOP_DATA_AZIMUTH_RCS_MAP_H

#include "abstract_rcs_map.h"
#include "az_data.h"

class AzimuthRcsDataCollection : public AbstractDataCollection {
private:
    map<height_t, map<azimuth_t, AzimuthRcsDataSet>> _data;
    vector<height_t> _heights;

public:
    explicit AzimuthRcsDataCollection(const path& input_path);

    void use_filtered_peaks();

    [[nodiscard]] const AbstractDataSet* get_for_exact_position(const Observer& observer) const override;

    [[nodiscard]] vector<height_t> heights() const;
};

#endif //RCSOP_DATA_AZIMUTH_RCS_MAP_H
