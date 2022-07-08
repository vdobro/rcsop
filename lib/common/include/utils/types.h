#ifndef RCSOP_COMMON_TYPES_H
#define RCSOP_COMMON_TYPES_H

#include <string>
#include <memory>
#include <functional>
#include <filesystem>
#include <map>
#include <vector>
#include <stdexcept>

using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;
using std::map;
using std::pair;
using std::make_pair;
using std::vector;
using std::function;
using std::invalid_argument;
using std::runtime_error;

using std::filesystem::path;
using std::filesystem::create_directories;
using std::filesystem::remove;

#endif //RCSOP_COMMON_TYPES_H
