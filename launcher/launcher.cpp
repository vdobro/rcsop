#include "launcher.h"

#include <iostream>

#include "utils/types.h"
#include "utils/task_utils.h"

#include "tasks/test_task.h"
#include "tasks/rcs_slices.h"
#include "tasks/rcs_sums.h"
#include "tasks/azimuth_rcs_plotter.h"
#include "tasks/sparse_filter.h"

#include "default_options.h"
#include "launcher_options.h"

namespace rcsop::launcher {
    using std::cin;
    using std::cout;
    using std::cerr;
    using std::clog;
    using std::endl;

    using std::filesystem::remove_all;
    using std::filesystem::is_directory;
    using std::filesystem::is_regular_file;

    using rcsop::common::utils::time::start_time;
    using rcsop::common::utils::time::log_and_start_next;

    using rcsop::data::InputDataCollector;

    using rcsop::launcher::parse_and_validate;

    const static map<string, launcher_task> available_tasks = {
            {"test-task",     rcsop::launcher::tasks::test_task},
            {"rcs-slices",    rcsop::launcher::tasks::rcs_slices},
            {"rcs-sums",      rcsop::launcher::tasks::accumulate_rcs},
            {"azimuth-sums",  rcsop::launcher::tasks::accumulate_azimuth},
            {DEFAULT_TASK,    rcsop::launcher::tasks::azimuth_rcs_plotter},
            {"sparse-filter", rcsop::launcher::tasks::sparse_filter},
    };

    int launcher_main(int argc, char** argv) {
        try {
            auto options = parse_and_validate(argc, argv, available_tasks);
            const auto& task_output_path = options.output_path;
            if (is_directory(task_output_path)) {
                cout << "Warning: directory '" << task_output_path.string()
                     << "' exists already and will be removed with ALL of its contents." << endl;
                cout << "This action is irreversible. Continue with deletion? [y/N] ";
                string confirmation;
                getline(cin, confirmation);
                if (!(confirmation == "y" || confirmation == "Y")) {
                    cout << "Task aborted, leaving existing folder intact. Exiting." << endl;
                    return EXIT_SUCCESS;
                }
                cout << "Removing contents of " << task_output_path.string() << endl;
                remove_all(task_output_path);
            }
            create_directories(task_output_path);

            InputDataCollector input_collector(options.input_path, options.camera);

            const auto task_executor = available_tasks.at(options.task_name);

            auto total_time = start_time();
            task_executor(input_collector, options);

            clog << endl;
            log_and_start_next(total_time, "Finished task '" + options.task_name + "', exiting.");
        } catch (const std::exception& e) {
            cerr << "Failed to execute given task, reason:" << endl;
            cerr << e.what() << endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
