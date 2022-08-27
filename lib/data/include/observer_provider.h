#ifndef RCSOP_DATA_CAMERA_PROVIDER_H
#define RCSOP_DATA_CAMERA_PROVIDER_H

#include "utils/types.h"

#include "input_data_collector.h"
#include "observer.h"

namespace rcsop::data {
    using rcsop::common::camera_options;

    class ObserverProvider {
    private:
        vector<Observer> _positioned_observers;
        vector<Observer> _auxiliary_observers;
        double _units_per_centimeter;

    public:
        ObserverProvider(const InputDataCollector& input,
                         const camera_options& camera_options,
                         bool fill_in_missing_observers = true);

        [[nodiscard]] auto observers_with_positions() const -> vector<Observer>;

        [[nodiscard]] auto all_observers() const -> vector<Observer>;

        [[nodiscard]] auto get_units_per_centimeter() const -> double;
    };
}

#endif //RCSOP_DATA_CAMERA_PROVIDER_H
