#ifndef RCSOP_DATA_CAMERA_PROVIDER_H
#define RCSOP_DATA_CAMERA_PROVIDER_H

#include "utils/types.h"

#include "input_data_collector.h"
#include "observer.h"

class ObserverProvider {
private:
    vector<Observer> _positioned_observers;
    vector<Observer> _auxiliary_observers;
    double _units_per_centimeter;

public:
    explicit ObserverProvider(const InputDataCollector& input,
                              const camera_options& camera_options);

    [[nodiscard]] vector<Observer> observers_with_positions() const;
    [[nodiscard]] vector<Observer> all_observers() const;

    [[nodiscard]] double get_units_per_centimeter() const;
};

#endif //RCSOP_DATA_CAMERA_PROVIDER_H
