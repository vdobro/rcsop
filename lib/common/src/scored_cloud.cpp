#include "scored_cloud.h"


namespace rcsop::common {
    ScoredCloud::ScoredCloud(const Observer& observer,
                             shared_ptr<vector<ScoredPoint>> points) :
            _observer(observer),
            _points(std::move(points)) {}

    const Observer& ScoredCloud::observer() const {
        return _observer;
    }

    shared_ptr<vector<ScoredPoint>> ScoredCloud::points() const {
        return this->_points;
    }

    auto multiple_scored_cloud_payload::observer_heights() const -> vector<height_t> {
        std::set<height_t> heights;
        for (const auto& cloud: point_clouds) {
            if (cloud.observer().has_position())
                heights.insert(cloud.observer().position().height);
        }
        vector<height_t> result;
        std::copy(heights.begin(), heights.end(), std::back_inserter(result));
        return result;
    }

    auto multiple_scored_cloud_payload::extract_single_payloads() const -> vector<scored_cloud_payload> {
        return utils::map_vec<ScoredCloud, scored_cloud_payload>(
                this->point_clouds,
                [this](const ScoredCloud& cloud) -> scored_cloud_payload {
                    return scored_cloud_payload{
                        .point_cloud = cloud,
                        .color_map = this->color_map,
                    };
                }
        );
    }
}
