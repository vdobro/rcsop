#pragma once

#include <vector>
#include <string>

using std::string;
using std::vector;

class rcs_data{
private:
    vector<double> _rcs;
    vector<double> _rcs_dbs;
    vector<long> _angles;
    vector<long> _ranges;

    rcs_data() = default;

public:
    explicit rcs_data(const string& path);

    [[nodiscard]] vector<double> rcs() const {
        return _rcs_dbs;
    }
};