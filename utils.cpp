#include "utils.h"


using namespace std;


int time_to_minutes(const std::string& time)
{
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}


void read_validate_positive_integer(std::ifstream& file, int& a)
{
    regex int_regex(R"(^[1-9]\d*$)");

    string line;
    if (!getline(file, line)) {
        cerr << line << endl;
        exit(0);
    }
    istringstream iss(line);
    string num_str;
    if (!(iss >> num_str) || !(regex_match(num_str, int_regex))) {
        cerr << line << endl;
        exit(0);
    }

    string extra;
    if (iss >> extra) {
        cerr << line << endl;
        exit(0);
    }

    a = stoi(num_str);
}


bool is_valid_time(const std::string& time) {
    regex time_regex(R"(^([01]\d|2[0-3]):[0-5]\d$)");
    return regex_match(time, time_regex);
}


void read_validate_times(std::ifstream& file, std::string& start_time, std::string& end_time) {

    std::string line;
    if (!std::getline(file, line)) {
        cerr << line << endl;
        exit(0);
    }

    std::istringstream iss(line);
    if (!(iss >> start_time >> end_time) || !is_valid_time(start_time) || !is_valid_time(end_time) || start_time >= end_time) {
        cerr << line << endl;
        exit(0);
    }

    std::string extra;
    if (iss >> extra) {
        cerr << line << endl;
        exit(0);
    }
}


bool is_valid_client_name(const std::string& name) {
    regex name_regex(R"(^[a-z0-9_-]+$)");
    return regex_match(name, name_regex);
}


bool is_time_non_decreasing(const std::string& a, const string& b) {
    return a <= b;
}