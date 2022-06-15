#ifndef RCSOP_DATA_CAMERA_PROVIDER_H
#define RCSOP_DATA_CAMERA_PROVIDER_H

#include "input_data_collector.h"
#include "observer.h"

class ObserverProvider {
private:
    vector<Observer> _observers;

public:
    explicit ObserverProvider(const InputDataCollector& input);

    [[nodiscard]] vector<Observer> observers() const;
};

#endif //RCSOP_DATA_CAMERA_PROVIDER_H
