#ifndef RCSOP_COMMON_TEXTURE_H
#define RCSOP_COMMON_TEXTURE_H

#include "utils/types.h"
#include "observer_position.h"

namespace rcsop::common {
    class Texture {
    private:
        path _file_path;
        ObserverPosition _camera_position;
    public:
        explicit Texture(path file_path,
                         const ObserverPosition& position);

        [[nodiscard]] ObserverPosition position() const;

        [[nodiscard]] path file_path() const;
    };
}

#endif //RCSOP_COMMON_TEXTURE_H
