#ifndef RCSOP_DATA_MODEL_WRITER_H
#define RCSOP_DATA_MODEL_WRITER_H

#include "input_data_collector.h"
#include "scored_cloud.h"
#include "sparse_cloud.h"
#include "dense_cloud.h"

namespace rcsop::data {
    using rcsop::data::InputDataCollector;
    using rcsop::common::scored_cloud_payload;
    using rcsop::common::SparseCloud;
    using rcsop::common::DenseCloud;

    class ModelWriter {
    private:
        bool _use_sparse = false;
        shared_ptr<SparseCloud> _sparse_model = nullptr;
        shared_ptr<DenseCloud> _dense_model = nullptr;


    public:
        ModelWriter(const InputDataCollector& inputs,
                    bool use_sparse_cloud);

        ~ModelWriter() = default;

        void add_points(const scored_cloud_payload& points);

        void write(const path& output_path);
    };
}

#endif //RCSOP_DATA_MODEL_WRITER_H
