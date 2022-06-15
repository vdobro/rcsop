#ifndef RCSO_DATA_CAMERA_PROVIDER_H
#define RCSO_DATA_CAMERA_PROVIDER_H

#include "input_data_collector.h"
#include "observer.h"

class ObserverProvider {
private:
    vector<Observer> _observers;

public:
    explicit ObserverProvider(const InputDataCollector& input);

    [[nodiscard]] vector<Observer> observers() const;
};


#endif //RCSO_DATA_CAMERA_PROVIDER_H
