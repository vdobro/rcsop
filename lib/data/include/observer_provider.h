#ifndef RCSOP_DATA_CAMERA_PROVIDER_H
#define RCSOP_DATA_CAMERA_PROVIDER_H

#include "utils/types.h"

#include "input_data_collector.h"
#include "observer.h"

class ObserverProvider {
private:
    vector<Observer> _positioned_observers;
    vector<Observer> _auxiliary_observers;

public:
    explicit ObserverProvider(const InputDataCollector& input,
                              double distance_to_origin,
                              CameraCorrectionParams default_observer_correction = {});

    [[nodiscard]] vector<Observer> observers_with_positions() const;
    [[nodiscard]] vector<Observer> all_observers() const;
};

#endif //RCSOP_DATA_CAMERA_PROVIDER_H
