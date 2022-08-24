#ifndef RCSOP_DATA_MODEL_WRITER_H
#define RCSOP_DATA_MODEL_WRITER_H

#include "scored_cloud.h"
#include "base_point_cloud.h"
#include "output_data_writer.h"
#include "model_camera.h"

namespace rcsop::data {
    using rcsop::common::scored_cloud_payload;
    using rcsop::common::BasePointCloud;
    using rcsop::common::OutputDataWriter;
    using rcsop::common::ObserverPosition;
    using rcsop::common::height_t;
    using rcsop::common::ModelCamera;

    class ModelWriter : public OutputDataWriter {
    private:
        optional<ObserverPosition> _observer_position;
        shared_ptr<BasePointCloud> _target_model = nullptr;
        size_t _point_count{};

    public:
        explicit ModelWriter(shared_ptr <BasePointCloud> target);

        void add_points(const scored_cloud_payload& points);

        void set_observer_position(ObserverPosition observer,
                                   const ModelCamera& observer_camera);

        [[nodiscard]] height_t observer_height() const override;

        void write(const path& output_path, const string& log_prefix) override;

        [[nodiscard]] bool observer_has_position() const override;

        [[nodiscard]] string path_prefix() const override;
    };
}

#endif //RCSOP_DATA_MODEL_WRITER_H
