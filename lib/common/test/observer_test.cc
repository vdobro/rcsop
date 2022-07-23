#include <gtest/gtest.h>

#include "utils/types.h"
#include "utils/sparse.h"
#include "observer.h"

using rcsop::common::Observer;
using rcsop::common::ObserverPosition;
using rcsop::common::camera;
using rcsop::common::camera_options;
using rcsop::common::utils::sparse::Image;
using rcsop::common::utils::sparse::Camera;
using rcsop::common::utils::sparse::Reconstruction;

class ObserverShould : public ::testing::Test {
protected:
    const int _camera_model_id = 1;
    const rcsop::common::camera_id_t _camera_id = 2;
    const colmap::image_t _image_id = 3;

    const double DISTANCE_TO_ORIGIN_CM = 100.;
    const double DISTANCE_UNITS = 2.;
    const double UNITS_PER_CENTIMETER = DISTANCE_UNITS / DISTANCE_TO_ORIGIN_CM;

    const ObserverPosition _position = ObserverPosition{
            .height = 40,
            .azimuth = 20,
    };
    camera_options _camera_options = {
            .pitch_correction = 0.,
            .distance_to_origin = DISTANCE_TO_ORIGIN_CM,
            .default_height = 40,
    };
    path _camera_path = path{"camera.png"};

    shared_ptr<Camera> _camera;
    shared_ptr<Image> _image;
    shared_ptr<Reconstruction> _model;

    shared_ptr<Observer> _sut;

    ObserverShould() {
        _camera = make_shared<Camera>();
        _camera->SetModelId(_camera_model_id);
        _camera->SetCameraId(_camera_id);

        _image = make_shared<Image>();
        _image->SetImageId(_image_id);
        _image->SetName(_camera_path.string());
        _image->SetCameraId(_camera->CameraId());

        _model = make_shared<Reconstruction>();
        _model->AddCamera(*_camera);
        _model->AddImage(*_image);

        auto observer_camera = camera(*_image, *_model);

        _sut = make_shared<Observer>(optional<ObserverPosition>(_position),
                                     _camera_path,
                                     observer_camera,
                                     _camera_options);
        _sut->set_units_per_centimeter(UNITS_PER_CENTIMETER);
    };
};

TEST_F(ObserverShould, SetsPropertiesCorrectly) {
    EXPECT_EQ(_sut->has_position(), true);
    auto position = _sut->position();
    EXPECT_EQ(position.azimuth, _position.azimuth);
    EXPECT_EQ(position.height, _position.height);
}

TEST_F(ObserverShould, CalculateVerticalAngle) {
    //TODO
}

TEST_F(ObserverShould, CalculateHorizontalAngle) {
    //TODO
}

TEST_F(ObserverShould, CalculateBothAngles) {
    //TODO
}
