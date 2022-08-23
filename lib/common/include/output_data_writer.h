#ifndef RCSOP_COMMON_OUTPUT_DATA_WRITER_H
#define RCSOP_COMMON_OUTPUT_DATA_WRITER_H

#include "utils/types.h"
#include "observer_position.h"

namespace rcsop::common {
    using rcsop::common::height_t;

    class OutputDataWriter {
    protected:
        OutputDataWriter() = default;

    public:
        virtual ~OutputDataWriter() = default;

        virtual void write(const path& output_path,
                           const string& log_prefix) = 0;

        [[nodiscard]] virtual bool observer_has_position() const = 0;

        [[nodiscard]] virtual height_t observer_height() const = 0;

        [[nodiscard]] virtual string path_prefix() const = 0;
    };
}
#endif //RCSOP_COMMON_OUTPUT_DATA_WRITER_H
