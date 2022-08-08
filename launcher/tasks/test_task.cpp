#include "test_task.h"
#include "observer_provider.h"
#include "default_options.h"

#include <iostream>

namespace rcsop::launcher::tasks {
    using std::clog;
    using std::endl;

    using rcsop::data::ObserverProvider;
    using rcsop::data::AZIMUTH_RCS_MINIMAP;

    void test_task(const InputDataCollector& inputs,
                   const rcsop::launcher::utils::task_options& options) {
        clog << "Image file paths: " << endl;
        for (const auto& image: inputs.images()) {
            clog << image.image_name() << endl;
        }

        clog << "Observer positions: " << endl;
        auto observer_provider = make_shared<ObserverProvider>(inputs, options.camera);
        auto observers = observer_provider->observers_with_positions();
        for (const auto& observer: observers) {
            if (observer.has_position()) {
                clog << observer.position().str() << endl;
            }
        }

        clog << "RCS minimap paths: " << endl;
        auto minimaps = inputs.data<AZIMUTH_RCS_MINIMAP>(false);
        for (const auto& observer: observers) {
            clog << minimaps->for_position(observer).file_path().filename() << endl;
        }

        clog << "Test task done" << endl;
    }
}
