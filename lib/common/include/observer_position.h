#ifndef RCSOP_COMMON_OBSERVER_POSITION_H
#define RCSOP_COMMON_OBSERVER_POSITION_H

typedef long height_t;
typedef long azimuth_t;

struct ObserverPosition {
    height_t height;
    azimuth_t azimuth;

    string str() const {
        return "height = " + std::to_string(height) + ", angle = " + std::to_string(azimuth);
    }
};

#endif //RCSOP_COMMON_OBSERVER_POSITION_H
