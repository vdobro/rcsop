#pragma once

#include <string>
#include <vector>
#include <matio.h>

mat_t* open_mat_file(const std::string& path);

void close_mat_file(mat_t* file_handle);

matvar_t* get_table(mat_t* file);

unsigned int get_row_for_height(unsigned int height, matvar_t* table);

std::vector<double> get_rcs(unsigned int index, matvar_t* table);

std::vector<long> get_ranges(unsigned int range, matvar_t* table);

std::vector<double> get_rcs_db(unsigned int index, matvar_t* table);

std::vector<long> get_angles(unsigned int index, matvar_t* table);