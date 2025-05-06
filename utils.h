#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <regex>
#include <algorithm>


int time_to_minutes(const std::string& time);
void read_validate_positive_integer(std::ifstream& file, int& a);
bool is_valid_time(const std::string& time);
void read_validate_times(std::ifstream& file, std::string& start_time, std::string& end_time);


bool is_valid_client_name(const std::string& name);
bool is_time_non_decreasing(const std::string& a, const std::string& b);

#endif