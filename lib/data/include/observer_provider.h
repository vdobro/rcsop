#ifndef RCSOP_DATA_CAMERA_PROVIDER_H
#define RCSOP_DATA_CAMERA_PROVIDER_H

#include "utils/types.h"

#include "input_data_collector.h"
#include "observer.h"

class ObserverProvider {
private:
    vector<Observer> _observers;

public:
    explicit ObserverProvider(const InputDataCollector& input,
                              double distance_to_origin,
                              CameraCorrectionParams default_observer_correction = {});

    [[nodiscard]] vector<Observer> observers() const;
};

#endif //RCSOP_DATA_CAMERA_PROVIDER_H
