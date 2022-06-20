#include "test_task.h"
#include "observer_provider.h"

#include <iostream>

using std::clog;
using std::endl;

void dummy_task(const shared_ptr<InputDataCollector>& inputs,
                const path& output_path) {
    clog << "Image file paths: " << endl;
    for (const auto& image: inputs->images()) {
        clog << image.file_path() << endl;
    }

    clog << "Observer positions: " << endl;
    auto observer_provider = make_shared<ObserverProvider>(*inputs);
    auto observers = observer_provider->observers();
    for (const auto& observer: observers) {
        clog << observer.position().str() << endl;
    }

    clog << "RCS minimap paths: " << endl;
    shared_ptr<AzimuthMinimapProvider> minimaps = inputs->data<AZIMUTH_RCS_MINIMAP>(false);
    for (const auto& observer: observers) {
        clog << minimaps->at_position(observer.position())->file_path() << endl;
    }

    clog << "Test task done" << endl;
}
