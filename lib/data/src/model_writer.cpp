#include "model_writer.h"

namespace rcsop::data {

    ModelWriter::ModelWriter(const InputDataCollector& inputs,
                             bool use_sparse_cloud) : _use_sparse(use_sparse_cloud) {
        if (_use_sparse) {
            _sparse_model = inputs.data<SPARSE_CLOUD_COLMAP>();
        } else {
            _dense_model = inputs.data<DENSE_MESH_PLY>();
        }
    }


    void ModelWriter::add_points(const scored_cloud_payload& payload) {
        for (const auto& point : *(payload.point_cloud.points())) {
            if (_use_sparse) {
                _sparse_model->add_point(point.position(), payload.colormap(point.score_to_dB()));
            } else {
                //TODO
            }
        }
    }

    void ModelWriter::write(const path& output_path) {
        //TODO
    }
}
