#ifndef RCSOP_COMMON_TYPES_H
#define RCSOP_COMMON_TYPES_H

#include <cmath>
#include <string>
#include <memory>
#include <functional>
#include <filesystem>
#include <map>
#include <vector>
#include <set>
#include <stdexcept>
#include <optional>
#include <utility>
#include <regex>

using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;
using std::move;

using std::string;
using std::map;
using std::pair;
using std::make_pair;
using std::vector;
using std::set;

using std::function;
using std::optional;

using std::invalid_argument;
using std::runtime_error;

using std::abs;
using std::min;
using std::max;

using std::regex;
using std::smatch;

using std::filesystem::path;
using std::filesystem::create_directories;
using std::filesystem::remove;
using std::filesystem::recursive_directory_iterator;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

const double STANDARD_ERROR = 2E-8;

#define assert_near(vec_a, vec_b) assert(abs(((vec_a) - (vec_b)).norm()) < STANDARD_ERROR);

#endif //RCSOP_COMMON_TYPES_H
