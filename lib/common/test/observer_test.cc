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
using rcsop::common::utils::points::vec3_spherical;
using rcsop::common::utils::map_vec;

struct observation {
    double vertical_angle{};
    double horizontal_angle{};
    double distance{};
};

using observed_point_property_selector = std::function<double(const observed_point&)>;
using observation_property_selector = std::function<double(const observation&)>;
using projected_point_property_selector = std::function<double(const vec3&)>;

const double STANDARD_ERROR = 2E-8;

class MockObserverCamera : public ObserverCamera {
public:
    MOCK_METHOD(vec3, map_to_observer_local, (const vec3&), (const, override));
    MOCK_METHOD(vec3, map_to_world, (const vec3&), (const, override));
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
    const double UNIT_DISTANCE = 1 / UNITS_PER_CENTIMETER;
    const double DIAGONAL_DISTANCE = UNIT_DISTANCE * M_SQRT2;

    const ObserverPosition _position{
            .height = 40,
            .azimuth = 20,
    };

    const vec3 _observer_position{2, 3, 4};

    const vector<vec3> POINTS_PLANE_CENTER{
            {-1., 0,   0.},     // left
            {1.,  0,   0.},     // right
            {0.,  -1., 0.},     // behind
            {0.,  1.,  0.},     // front
            {0.,  0,   -1.},    // below
            {0.,  0,   1.},     // above
    };

    const vector<observation> POSITIONS_FACE_CENTER{
            {0.,   -90., UNIT_DISTANCE},
            {0.,   90.,  UNIT_DISTANCE},
            {0.,   0.,   UNIT_DISTANCE},
            {0.,   0.,   UNIT_DISTANCE},
            {-90., 0.,   UNIT_DISTANCE},
            {90.,  0.,   UNIT_DISTANCE},
    };

    constexpr static double R = M_SQRT1_2;
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

    const vector<observation> POSITIONS_EDGE_CENTER{
            {0.,   -45., UNIT_DISTANCE},
            {0.,   -45., UNIT_DISTANCE},
            {0.,   45.,  UNIT_DISTANCE},
            {0.,   45.,  UNIT_DISTANCE},
            {-45., 0.,   UNIT_DISTANCE},
            {45.,  0.,   UNIT_DISTANCE},
            {-45., 0.,   UNIT_DISTANCE},
            {45.,  0.,   UNIT_DISTANCE},
            {-45., -90., UNIT_DISTANCE},
            {-45., 90.,  UNIT_DISTANCE},
            {45.,  -90., UNIT_DISTANCE},
            {45.,  90,   UNIT_DISTANCE},
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

    const vector<observation> POSITIONS_EDGE_VERTICES{
            {-45., -45., DIAGONAL_DISTANCE},
            {45.,  -45., DIAGONAL_DISTANCE},
            {-45., -45., DIAGONAL_DISTANCE},
            {45.,  -45., DIAGONAL_DISTANCE},
            {-45., 45.,  DIAGONAL_DISTANCE},
            {45.,  45.,  DIAGONAL_DISTANCE},
            {-45., 45.,  DIAGONAL_DISTANCE},
            {45.,  45.,  DIAGONAL_DISTANCE},
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
                .WillRepeatedly([this](const vec3& world_point) {
                    return world_point - this->_observer_position;
                });
        EXPECT_CALL(*_mock_camera, map_to_world(_))
                .WillRepeatedly([this](const vec3& local_point) {
                    return this->_observer_position + local_point;
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
            const vector<observation>& expected_values,
            const observation_property_selector& observation_selector
    ) {
        const auto offset_inputs = map_vec<vec3, vec3>(
                test_input, [this](const vec3& x) {
                    return x + this->_observer_position;
                });
        const auto result = observe_points(offset_inputs);
        for (size_t i = 0; i < result->size(); i++) {
            const auto result_value = selector(result->at(i));
            const auto expected_value = observation_selector(expected_values.at(i));
            EXPECT_NEAR(result_value, expected_value, STANDARD_ERROR);
        }
    }

    void project_and_check(
            const vector<observed_point>& test_input,
            const projected_point_property_selector& selector,
            const vector<vec3>& expected_values
    ) {
        const auto result = _sut->project_observed_positions(test_input);
        for (size_t i = 0; i < result->size(); i++) {
            const auto result_value = selector(result->at(i));
            const auto expected_value = expected_values.at(i) - this->_observer_position;
            EXPECT_NEAR(result_value, selector(expected_value), STANDARD_ERROR);
        }
    }
};

TEST_F(ObserverShould, SetPropertiesCorrectly) {
    EXPECT_EQ(_sut->has_position(), true);
    auto position = _sut->position();
    EXPECT_EQ(position.azimuth, _position.azimuth);
    EXPECT_EQ(position.height, _position.height);
}

TEST_F(ObserverShould, OutputAsManyPointsAsInput) {
    const auto& points = ALL_POINTS;
    auto result = observe_points(points);
    EXPECT_THAT(result->size(), points.size());
}


TEST_F(ObserverShould, CalculateVerticalAngleToFaceCenter) {
    observe_and_check(
            POINTS_PLANE_CENTER,
            &observed_point::vertical_angle,
            POSITIONS_FACE_CENTER,
            &observation::vertical_angle
    );
}

TEST_F(ObserverShould, CalculateHorizontalAngleToFaceCenter) {
    observe_and_check(
            POINTS_PLANE_CENTER,
            &observed_point::horizontal_angle,
            POSITIONS_FACE_CENTER,
            &observation::horizontal_angle
    );
}

TEST_F(ObserverShould, CalculateVerticalAngleToEdgeCenter) {
    observe_and_check(
            POINTS_EDGE_CENTER,
            &observed_point::vertical_angle,
            POSITIONS_EDGE_CENTER,
            &observation::vertical_angle
    );
}

TEST_F(ObserverShould, CalculateHorizontalAngleToEdgeCenter) {
    observe_and_check(
            POINTS_EDGE_CENTER,
            &observed_point::horizontal_angle,
            POSITIONS_EDGE_CENTER,
            &observation::horizontal_angle
    );
}

TEST_F(ObserverShould, CalculateVerticalAngleToEdgeVertices) {
    observe_and_check(
            POINTS_VERTICES,
            &observed_point::vertical_angle,
            POSITIONS_EDGE_VERTICES,
            &observation::vertical_angle
    );
}

TEST_F(ObserverShould, CalculateHorizontalAngleToEdgeVertices) {
    observe_and_check(
            POINTS_VERTICES,
            &observed_point::horizontal_angle,
            POSITIONS_EDGE_VERTICES,
            &observation::horizontal_angle
    );
}

TEST_F(ObserverShould, CalculateVerticalAngleOfOrigin) {
    observe_and_check(
            vector<vec3>{vec3::Zero()},
            &observed_point::vertical_angle,
            vector<observation>{{}},
            &observation::vertical_angle
    );
}


TEST_F(ObserverShould, CalculateHorizontalAngleToOrigin) {
    observe_and_check(
            vector<vec3>{vec3::Zero()},
            &observed_point::horizontal_angle,
            vector<observation>{{}},
            &observation::horizontal_angle
    );
}

TEST_F(ObserverShould, CalculateDistance) {
    observe_and_check(POINTS_PLANE_CENTER, &observed_point::distance_in_world,
                      POSITIONS_FACE_CENTER, &observation::distance);

    observe_and_check(POINTS_EDGE_CENTER, &observed_point::distance_in_world,
                      POSITIONS_EDGE_CENTER, &observation::distance);

    observe_and_check(POINTS_VERTICES, &observed_point::distance_in_world,
                      POSITIONS_EDGE_VERTICES, &observation::distance);
}

TEST_F(ObserverShould, CalculateDistanceEqualToSphericalRadialComponent) {
    rcsop::common::point_id_t id{0};
    for (const auto& point: ALL_POINTS) {
        const auto observed = _sut->observe_point(ScoredPoint(point, id++, 0.));
        const auto spherical = Observer::cartesian_to_spherical(this->_mock_camera->map_to_observer_local(point));

        const auto distance = observed.distance_in_world * UNITS_PER_CENTIMETER;
        const auto radial = spherical.radial;

        EXPECT_NEAR(distance, radial, STANDARD_ERROR);
    }
}

using uniform_distribution = std::uniform_real_distribution<double>;
TEST_F(ObserverShould, ConvertSphericalToCartesianToSpherical) {
    std::random_device rd;
    std::mt19937 e2(rd());
    uniform_distribution radius_distribution(0, 10);
    uniform_distribution theta_distribution(0, M_PI);
    uniform_distribution phi_distribution(0, M_2_PI - STANDARD_ERROR);

    for (size_t i{0}; i < 750; i++) {
        vec3_spherical input{
                .radial = radius_distribution(e2),
                .azimuthal = theta_distribution(e2),
                .polar = phi_distribution(e2),
        };
        auto cartesian = Observer::spherical_to_cartesian(input);
        auto result = Observer::cartesian_to_spherical(cartesian);

        EXPECT_NEAR(input.radial, result.radial, STANDARD_ERROR);
        EXPECT_NEAR(input.azimuthal, result.azimuthal, STANDARD_ERROR);
        EXPECT_NEAR(input.polar, result.polar, STANDARD_ERROR);
    }
}

TEST_F(ObserverShould, ConvertCartesianToSphericalToCartesian) {
    std::random_device rd;
    std::mt19937 e2(rd());
    uniform_distribution real_distribution(-10, 10);

    for (size_t i{0}; i < 750; i++) {
        vec3 input{
                real_distribution(e2),
                real_distribution(e2),
                real_distribution(e2),
        };
        auto spherical = Observer::cartesian_to_spherical(input);
        auto result = Observer::spherical_to_cartesian(spherical);

        EXPECT_NEAR(input.x(), result.x(), STANDARD_ERROR);
        EXPECT_NEAR(input.y(), result.y(), STANDARD_ERROR);
        EXPECT_NEAR(input.z(), result.z(), STANDARD_ERROR);
    }
}

TEST_F(ObserverShould, ProjectPointToFaceCenter) {

}
