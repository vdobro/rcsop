#include <gtest/gtest.h>
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Gt;
using ::testing::Return;

#include <utility>

#include "utils/types.h"
#include "utils/mapping.h"
#include "utils/sparse.h"
#include "observer.h"

using rcsop::common::Observer;
using rcsop::common::ScoredPoint;
using rcsop::common::observed_point;

using rcsop::common::ObserverPosition;
using rcsop::common::ObserverCamera;
using rcsop::common::camera;
using rcsop::common::camera_options;
using rcsop::common::utils::sparse::Image;
using rcsop::common::utils::sparse::Camera;
using rcsop::common::utils::sparse::Reconstruction;
using rcsop::common::utils::points::vec3;
using rcsop::common::utils::map_vec;

using observed_point_property_selector = std::function<double(const observed_point&)>;

const double STANDARD_ERROR = 2E-8;

class MockObserverCamera : public ObserverCamera {
public:
    MOCK_METHOD(vec3, map_to_observer_local, (const vec3&), (const, override));
    MOCK_METHOD(double, distance_to_camera, (const vec3&), (const, override));
    MOCK_METHOD(camera, native_camera, (), (const, override));
};

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

    const vec3 _observer_position = vec3(2, 3, 4);

    const vector<vec3> POINTS_PLANE_CENTER{
            {-1., 0,   0.},     // left
            {1.,  0,   0.},     // right
            {0.,  -1., 0.},     // behind
            {0.,  1.,  0.},     // front
            {0.,  0,   -1.},    // below
            {0.,  0,   1.},     // above
    };

    const double R = M_SQRT1_2;
    const vector<vec3> POINTS_EDGE_CENTER{
            {-R, -R, 0.},   // left behind
            {-R, R,  0.},   // left front
            {R,  -R, 0.},   // right behind
            {R,  R,  0.},   // right front
            {0., -R, -R},   // behind below
            {0., -R, R},    // behind above
            {0., R,  -R},   // front below
            {0., R,  R},    // front above
            {-R, 0., -R},   // left below
            {R,  0., -R},   // right below
            {-R, 0., R},   // left above
            {R,  0., R},   // right above
    };

    const vector<vec3> POINTS_VERTICES{
            {-R, -R, -1},   // left behind below
            {-R, -R, 1},    // left behind above
            {-R, R,  -1},   // left front below
            {-R, R,  1},    // left front above
            {R,  -R, -1},   // right behind below
            {R,  -R, 1},    // right behind above
            {R,  R,  -1},   // right front below
            {R,  R,  1},    // right front above
    };

    vector<vec3> ALL_POINTS;

    shared_ptr<MockObserverCamera> _mock_camera = make_shared<MockObserverCamera>();

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

        EXPECT_CALL(*_mock_camera, map_to_observer_local(_))
                .WillRepeatedly([this](const vec3& point) {
                    return point - this->_observer_position;
                });
        EXPECT_CALL(*_mock_camera, distance_to_camera(_))
                .WillRepeatedly([this](const vec3& point) {
                    return (point - this->_observer_position).norm();
                });

        _sut = make_shared<Observer>(
                optional<ObserverPosition>(_position),
                _camera_path,
                _mock_camera);
        _sut->set_units_per_centimeter(UNITS_PER_CENTIMETER);

        ALL_POINTS.insert(ALL_POINTS.end(), POINTS_PLANE_CENTER.begin(), POINTS_PLANE_CENTER.end());
        ALL_POINTS.insert(ALL_POINTS.end(), POINTS_EDGE_CENTER.begin(), POINTS_EDGE_CENTER.end());
        ALL_POINTS.insert(ALL_POINTS.end(), POINTS_VERTICES.begin(), POINTS_VERTICES.end());
    };

    shared_ptr<vector<observed_point>> observe_points(
            const vector<vec3>& test_input) {
        const auto input = map_vec<vec3, ScoredPoint>(test_input, [](const vec3& point) {
            return ScoredPoint(point, 0);
        });
        return _sut->observe_points(input);
    }

    void observe_and_check(
            const vector<vec3>& test_input,
            const observed_point_property_selector& selector,
            const vector<double>& expected_values
    ) {
        const auto offset_inputs = map_vec<vec3, vec3>(
                test_input, [this](const vec3& x) {
                    return x + this->_observer_position;
                });
        const auto result = observe_points(offset_inputs);
        for (size_t i = 0; i < result->size(); i++) {
            const auto result_value = selector(result->at(i));
            const auto expected_value = expected_values.at(i);
            EXPECT_NEAR(result_value, expected_value, STANDARD_ERROR);
        }
    }
};

TEST_F(ObserverShould, SetsPropertiesCorrectly) {
    EXPECT_EQ(_sut->has_position(), true);
    auto position = _sut->position();
    EXPECT_EQ(position.azimuth, _position.azimuth);
    EXPECT_EQ(position.height, _position.height);
}

TEST_F(ObserverShould, OutputsAsManyPointsAsInput) {
    const auto& points = ALL_POINTS;
    auto result = observe_points(points);
    EXPECT_THAT(result->size(), points.size());
}

const auto& select_vertical_angle = [](const observed_point& x) {
    return x.vertical_angle;
};

TEST_F(ObserverShould, CalculateVerticalAngleToFaceCenter) {
    observe_and_check(
            POINTS_PLANE_CENTER,
            select_vertical_angle,
            vector<double>{
                    0., 0.,
                    0., 0.,
                    -90., 90.,
            }
    );
}

TEST_F(ObserverShould, CalculateVerticalAngleToEdgeCenter) {
    observe_and_check(
            POINTS_EDGE_CENTER,
            select_vertical_angle,
            vector<double>{
                    0., 0.,
                    0., 0.,
                    -45., 45.,
                    -45., 45.,
                    -45., -45.,
                    45., 45.,
            }
    );
}

TEST_F(ObserverShould, CalculateVerticalAngleToEdgeVertices) {
    observe_and_check(
            POINTS_VERTICES,
            select_vertical_angle,
            vector<double>{
                    -45., 45.,
                    -45., 45.,
                    -45., 45.,
                    -45., 45.,
            }
    );
}

TEST_F(ObserverShould, CalculateVerticalAngleOfOrigin) {
    observe_and_check(vector<vec3>{vec3::Zero()},
                      select_vertical_angle,
                      vector<double>{0.});
}

const auto select_horizontal_angle = [](const observed_point& x) {
    return x.horizontal_angle;
};

TEST_F(ObserverShould, CalculateHorizontalAngleToFaceCenter) {
    observe_and_check(
            POINTS_PLANE_CENTER,
            select_horizontal_angle,
            vector<double>{
                    -90., 90.,
                    0., 0.,
                    0., 0.,
            }
    );
}

TEST_F(ObserverShould, CalculateHorizontalAngleToEdgeCenter) {
    observe_and_check(
            POINTS_EDGE_CENTER,
            select_horizontal_angle,
            vector<double>{
                    -45., -45.,
                    45., 45.,
                    0., 0.,
                    0., 0.,
                    -90., 90.,
                    -90., 90.,
            }
    );
}

TEST_F(ObserverShould, CalculateHorizontalAngleToEdgeVertices) {
    observe_and_check(
            POINTS_VERTICES,
            select_horizontal_angle,
            vector<double>{
                    -45., -45.,
                    -45., -45.,
                    45., 45.,
                    45., 45.,
            }
    );
}

TEST_F(ObserverShould, CalculateHorizontalAngleToOrigin) {
    observe_and_check(vector<vec3>{vec3::Zero()},
                      select_horizontal_angle,
                      vector<double>{0.});
}

TEST_F(ObserverShould, CalculateDistance) {
    const auto unit_distance = 1 / UNITS_PER_CENTIMETER;
    const auto diagonal_distance = unit_distance * M_SQRT2;
    const auto select_distance = [](const observed_point& x) {
        return x.distance_in_world;
    };

    vector<double> straight_distances(POINTS_PLANE_CENTER.size(), unit_distance);
    observe_and_check(POINTS_PLANE_CENTER, select_distance, straight_distances);

    vector<double> edge_center_distances(POINTS_EDGE_CENTER.size(), unit_distance);
    observe_and_check(POINTS_EDGE_CENTER, select_distance, edge_center_distances);

    vector<double> vertex_distances(POINTS_VERTICES.size(), diagonal_distance);
    observe_and_check(POINTS_VERTICES, select_distance, vertex_distances);
}
