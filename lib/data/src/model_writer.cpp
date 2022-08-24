#include "model_writer.h"

#include <utility>
#include "utils/chronometer.h"

namespace rcsop::data {
    using rcsop::common::utils::time::start_time;
    using rcsop::common::utils::time::log_and_start_next;

    ModelWriter::ModelWriter(shared_ptr<BasePointCloud> target)
            : _target_model(std::move(target)) {
        _point_count = _target_model->point_count();
    }

    void ModelWriter::add_points(const scored_cloud_payload& payload) {
        for (const auto& point: *(payload.point_cloud.points())) {
            _target_model->add_point(
                    point.position(),
                    payload.colormap(point.score_to_dB())
            );
        }

        _point_count = _target_model->point_count();
    }

    void ModelWriter::write(const path& output_path,
                            const string& log_prefix) {
        auto time_measure = start_time();

        if (!observer_has_position()) {
            throw std::domain_error("Observer position needed for the output directory name");
        }
        auto target_folder_name = _observer_position->str();
        path target_path{output_path / target_folder_name};

        create_directories(target_path);
        _target_model->write(target_path);

        log_and_start_next(time_measure, log_prefix + "\tOutput sparse cloud model " + target_folder_name
                                         + " with a total of " + std::to_string(this->_point_count) + " points");
    }

    string ModelWriter::path_prefix() const {
        return "sparse";
    }

    bool ModelWriter::observer_has_position() const {
        return _observer_position.has_value();
    }

    height_t ModelWriter::observer_height() const {
        if (!observer_has_position()) {
            throw std::domain_error("Observer position not set.");
        }
        return this->_observer_position.value().height;
    }

    void ModelWriter::set_observer_position(const ObserverPosition position,
                                            const ModelCamera& observer_camera) {
        if (this->_observer_position.has_value()) {
            throw std::domain_error("Position set already");
        }
        this->_observer_position = position;
        this->_target_model->purge_cameras(observer_camera.id());
    }
}
