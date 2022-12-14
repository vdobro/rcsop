#include "colors.h"

#include <cmath>

namespace rcsop::common::coloring {
    using rcsop::common::utils::sparse::color_vec;
    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::min_value;
    using rcsop::common::utils::max_value;

    using std::lround;

    const static unsigned char RGB_MAX = 255;
    const static double RGB_MAX_D = 255.;

    // Copyright 2019 Google LLC.
    // SPDX-License-Identifier: Apache-2.0

    // Author: Anton Mikhailov

    // The look-up tables contains 256 entries. Each entry is a an sRGB triplet.
    const static unsigned char turbo_srgb_bytes[256][3] = {{48,  18,  59},
                                                           {50,  21,  67},
                                                           {51,  24,  74},
                                                           {52,  27,  81},
                                                           {53,  30,  88},
                                                           {54,  33,  95},
                                                           {55,  36,  102},
                                                           {56,  39,  109},
                                                           {57,  42,  115},
                                                           {58,  45,  121},
                                                           {59,  47,  128},
                                                           {60,  50,  134},
                                                           {61,  53,  139},
                                                           {62,  56,  145},
                                                           {63,  59,  151},
                                                           {63,  62,  156},
                                                           {64,  64,  162},
                                                           {65,  67,  167},
                                                           {65,  70,  172},
                                                           {66,  73,  177},
                                                           {66,  75,  181},
                                                           {67,  78,  186},
                                                           {68,  81,  191},
                                                           {68,  84,  195},
                                                           {68,  86,  199},
                                                           {69,  89,  203},
                                                           {69,  92,  207},
                                                           {69,  94,  211},
                                                           {70,  97,  214},
                                                           {70,  100, 218},
                                                           {70,  102, 221},
                                                           {70,  105, 224},
                                                           {70,  107, 227},
                                                           {71,  110, 230},
                                                           {71,  113, 233},
                                                           {71,  115, 235},
                                                           {71,  118, 238},
                                                           {71,  120, 240},
                                                           {71,  123, 242},
                                                           {70,  125, 244},
                                                           {70,  128, 246},
                                                           {70,  130, 248},
                                                           {70,  133, 250},
                                                           {70,  135, 251},
                                                           {69,  138, 252},
                                                           {69,  140, 253},
                                                           {68,  143, 254},
                                                           {67,  145, 254},
                                                           {66,  148, 255},
                                                           {65,  150, 255},
                                                           {64,  153, 255},
                                                           {62,  155, 254},
                                                           {61,  158, 254},
                                                           {59,  160, 253},
                                                           {58,  163, 252},
                                                           {56,  165, 251},
                                                           {55,  168, 250},
                                                           {53,  171, 248},
                                                           {51,  173, 247},
                                                           {49,  175, 245},
                                                           {47,  178, 244},
                                                           {46,  180, 242},
                                                           {44,  183, 240},
                                                           {42,  185, 238},
                                                           {40,  188, 235},
                                                           {39,  190, 233},
                                                           {37,  192, 231},
                                                           {35,  195, 228},
                                                           {34,  197, 226},
                                                           {32,  199, 223},
                                                           {31,  201, 221},
                                                           {30,  203, 218},
                                                           {28,  205, 216},
                                                           {27,  208, 213},
                                                           {26,  210, 210},
                                                           {26,  212, 208},
                                                           {25,  213, 205},
                                                           {24,  215, 202},
                                                           {24,  217, 200},
                                                           {24,  219, 197},
                                                           {24,  221, 194},
                                                           {24,  222, 192},
                                                           {24,  224, 189},
                                                           {25,  226, 187},
                                                           {25,  227, 185},
                                                           {26,  228, 182},
                                                           {28,  230, 180},
                                                           {29,  231, 178},
                                                           {31,  233, 175},
                                                           {32,  234, 172},
                                                           {34,  235, 170},
                                                           {37,  236, 167},
                                                           {39,  238, 164},
                                                           {42,  239, 161},
                                                           {44,  240, 158},
                                                           {47,  241, 155},
                                                           {50,  242, 152},
                                                           {53,  243, 148},
                                                           {56,  244, 145},
                                                           {60,  245, 142},
                                                           {63,  246, 138},
                                                           {67,  247, 135},
                                                           {70,  248, 132},
                                                           {74,  248, 128},
                                                           {78,  249, 125},
                                                           {82,  250, 122},
                                                           {85,  250, 118},
                                                           {89,  251, 115},
                                                           {93,  252, 111},
                                                           {97,  252, 108},
                                                           {101, 253, 105},
                                                           {105, 253, 102},
                                                           {109, 254, 98},
                                                           {113, 254, 95},
                                                           {117, 254, 92},
                                                           {121, 254, 89},
                                                           {125, 255, 86},
                                                           {128, 255, 83},
                                                           {132, 255, 81},
                                                           {136, 255, 78},
                                                           {139, 255, 75},
                                                           {143, 255, 73},
                                                           {146, 255, 71},
                                                           {150, 254, 68},
                                                           {153, 254, 66},
                                                           {156, 254, 64},
                                                           {159, 253, 63},
                                                           {161, 253, 61},
                                                           {164, 252, 60},
                                                           {167, 252, 58},
                                                           {169, 251, 57},
                                                           {172, 251, 56},
                                                           {175, 250, 55},
                                                           {177, 249, 54},
                                                           {180, 248, 54},
                                                           {183, 247, 53},
                                                           {185, 246, 53},
                                                           {188, 245, 52},
                                                           {190, 244, 52},
                                                           {193, 243, 52},
                                                           {195, 241, 52},
                                                           {198, 240, 52},
                                                           {200, 239, 52},
                                                           {203, 237, 52},
                                                           {205, 236, 52},
                                                           {208, 234, 52},
                                                           {210, 233, 53},
                                                           {212, 231, 53},
                                                           {215, 229, 53},
                                                           {217, 228, 54},
                                                           {219, 226, 54},
                                                           {221, 224, 55},
                                                           {223, 223, 55},
                                                           {225, 221, 55},
                                                           {227, 219, 56},
                                                           {229, 217, 56},
                                                           {231, 215, 57},
                                                           {233, 213, 57},
                                                           {235, 211, 57},
                                                           {236, 209, 58},
                                                           {238, 207, 58},
                                                           {239, 205, 58},
                                                           {241, 203, 58},
                                                           {242, 201, 58},
                                                           {244, 199, 58},
                                                           {245, 197, 58},
                                                           {246, 195, 58},
                                                           {247, 193, 58},
                                                           {248, 190, 57},
                                                           {249, 188, 57},
                                                           {250, 186, 57},
                                                           {251, 184, 56},
                                                           {251, 182, 55},
                                                           {252, 179, 54},
                                                           {252, 177, 54},
                                                           {253, 174, 53},
                                                           {253, 172, 52},
                                                           {254, 169, 51},
                                                           {254, 167, 50},
                                                           {254, 164, 49},
                                                           {254, 161, 48},
                                                           {254, 158, 47},
                                                           {254, 155, 45},
                                                           {254, 153, 44},
                                                           {254, 150, 43},
                                                           {254, 147, 42},
                                                           {254, 144, 41},
                                                           {253, 141, 39},
                                                           {253, 138, 38},
                                                           {252, 135, 37},
                                                           {252, 132, 35},
                                                           {251, 129, 34},
                                                           {251, 126, 33},
                                                           {250, 123, 31},
                                                           {249, 120, 30},
                                                           {249, 117, 29},
                                                           {248, 114, 28},
                                                           {247, 111, 26},
                                                           {246, 108, 25},
                                                           {245, 105, 24},
                                                           {244, 102, 23},
                                                           {243, 99,  21},
                                                           {242, 96,  20},
                                                           {241, 93,  19},
                                                           {240, 91,  18},
                                                           {239, 88,  17},
                                                           {237, 85,  16},
                                                           {236, 83,  15},
                                                           {235, 80,  14},
                                                           {234, 78,  13},
                                                           {232, 75,  12},
                                                           {231, 73,  12},
                                                           {229, 71,  11},
                                                           {228, 69,  10},
                                                           {226, 67,  10},
                                                           {225, 65,  9},
                                                           {223, 63,  8},
                                                           {221, 61,  8},
                                                           {220, 59,  7},
                                                           {218, 57,  7},
                                                           {216, 55,  6},
                                                           {214, 53,  6},
                                                           {212, 51,  5},
                                                           {210, 49,  5},
                                                           {208, 47,  5},
                                                           {206, 45,  4},
                                                           {204, 43,  4},
                                                           {202, 42,  4},
                                                           {200, 40,  3},
                                                           {197, 38,  3},
                                                           {195, 37,  3},
                                                           {193, 35,  2},
                                                           {190, 33,  2},
                                                           {188, 32,  2},
                                                           {185, 30,  2},
                                                           {183, 29,  2},
                                                           {180, 27,  1},
                                                           {178, 26,  1},
                                                           {175, 24,  1},
                                                           {172, 23,  1},
                                                           {169, 22,  1},
                                                           {167, 20,  1},
                                                           {164, 19,  1},
                                                           {161, 18,  1},
                                                           {158, 16,  1},
                                                           {155, 15,  1},
                                                           {152, 14,  1},
                                                           {149, 13,  1},
                                                           {146, 11,  1},
                                                           {142, 10,  1},
                                                           {139, 9,   2},
                                                           {136, 8,   2},
                                                           {133, 7,   2},
                                                           {129, 6,   2},
                                                           {126, 5,   2},
                                                           {122, 4,   3}};

    static color_vec to_vec(const unsigned char* color) {
        color_vec vec;
        vec.setZero();
        vec.x() = color[0];
        vec.y() = color[1];
        vec.z() = color[2];
        vec.w() = RGB_MAX;
        return vec;
    }

    static color_vec floats_to_color(double r, double g, double b, double a) {
        color_vec color;
        color.x() = lround(r * RGB_MAX_D);
        color.y() = lround(g * RGB_MAX_D);
        color.z() = lround(b * RGB_MAX_D);
        color.w() = lround(a * RGB_MAX_D);
        return color;
    }

    color_vec map_turbo(double v, double vmin, double vmax) {
        if (v <= vmin) {
            return to_vec(*(turbo_srgb_bytes + 0));
        }
        if (v >= vmax) {
            return to_vec(*(turbo_srgb_bytes + RGB_MAX));
        }
        auto range = vmax - vmin;
        auto pos = v - vmin;
        auto step_size = range / RGB_MAX_D;
        auto steps = lround(pos / step_size);
        return to_vec(*(turbo_srgb_bytes + steps));
    }

    /*
     * Source: http://paulbourke.net/miscellaneous/colourspace/
     *
     * Return an RGB colour value given a scalar v in the range [vmin, vmax]
     * In this case each colour component ranges from 0 (no contribution) to
     * 1 (fully saturated), modifications for other ranges is trivial.
     * The colour is clipped at the end of the scales if v is outside
     * the range [vmin,vmax]
    */
    color_vec map_jet(double v, double vmin, double vmax) {
        double r = 1.0, g = 1.0, b = 1.0; // white
        double dv = vmax - vmin;

        if (v < vmin) {
            v = vmin;
        }
        if (v > vmax) {
            v = vmax;
        }

        if (v < (vmin + 0.25 * dv)) {
            r = 0;
            g = 4 * (v - vmin) / dv;
        } else if (v < (vmin + 0.5 * dv)) {
            r = 0;
            b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
        } else if (v < (vmin + 0.75 * dv)) {
            r = 4 * (v - vmin - 0.5 * dv) / dv;
            b = 0;
        } else {
            g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
            b = 0;
        }

        return floats_to_color(r, g, b, 1.);
    }

    static auto clamp_and_factor(double v, double vmin, double vmax) -> double {
        auto value = std::clamp(v, vmin, vmax);

        auto range = vmax - vmin;
        auto pos = value - vmin;
        auto factor = pos / range;
        return factor;
    }

    // just red
    static const double LINEAR_MAP_RED = 1.;
    static const double LINEAR_MAP_GREEN = 0.;
    static const double LINEAR_MAP_BLUE = 0.;
    static const double LINEAR_MAP_ALPHA = 1.;

    color_vec map_red(double v, double vmin, double vmax) {
        auto factor = clamp_and_factor(v, vmin, vmax);

        return floats_to_color(LINEAR_MAP_RED * factor,
                               LINEAR_MAP_GREEN * factor,
                               LINEAR_MAP_BLUE * factor,
                               LINEAR_MAP_ALPHA * factor);
    }

    color_vec map_red_alpha_only(double v, double vmin, double vmax) {
        auto factor = clamp_and_factor(v, vmin, vmax);

        return floats_to_color(LINEAR_MAP_RED,
                               LINEAR_MAP_GREEN,
                               LINEAR_MAP_BLUE,
                               LINEAR_MAP_ALPHA * factor);
    }

    global_colormap_func construct_color_map_function(
            const local_colormap_func& color_map,
            double min_value,
            double max_value) {

        return [min_value, max_value, color_map](double value) {
            return color_map(value, min_value, max_value);
        };
    }

    global_colormap_func construct_color_map_function(
            const local_colormap_func& color_map,
            const ScoreRange& range) {

        return construct_color_map_function(color_map, range.min, range.max);
    }

    vector<color_vec> color_values(
            const vector<double>& values,
            const local_colormap_func& colormap) {

        auto min = min_value(values);
        auto max = max_value(values);

        return map_vec<double, color_vec>(values, [colormap, min, max](double value) {
            return colormap(value, min, max);
        });
    }

    const static map<string, local_colormap_func> COLOR_MAPS = {
            {"jet",            rcsop::common::coloring::map_jet},
            {"turbo",          rcsop::common::coloring::map_turbo},
            {"red",            rcsop::common::coloring::map_red},
            {"red-alpha-only", rcsop::common::coloring::map_red_alpha_only},
    };

    local_colormap_func resolve_map_by_name(const string& name) {
        if (!COLOR_MAPS.contains(name)) {
            throw invalid_argument("No color map found");
        }
        return COLOR_MAPS.at(name);
    }
}