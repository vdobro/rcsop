#ifndef SFMRCS_AZIMUTH_ANGLES_H
#define SFMRCS_AZIMUTH_ANGLES_H

#include "utils/types.h"

#include "sparse_cloud.h"
#include "dense_cloud.h"
#include "render_points.h"

using sfm::rendering::point_display_payload;

shared_ptr<point_display_payload> display_azimuth(
        const shared_ptr<SparseCloud>& model,
        const shared_ptr<DenseCloud>& dense_model,
        const path& image_path,
        const path& data_path,
        const path& output_path);

void render_to_files(const point_display_payload& point_payload,
                     const path& output_path);

#endif //SFMRCS_AZIMUTH_ANGLES_H