#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include "ComputerClub.h"
#include "utils.h"


using namespace std;


bool process_event_details(std::istringstream& iss, Event& event, int tables_count) 
{
    if (event.id == 1 || event.id == 3 || event.id == 4) {
        if (!(iss >> event.client) || !is_valid_client_name(event.client)) {
            return false;
        }
    } else if (event.id == 2) { 
        if (!(iss >> event.client >> event.table_id) || !is_valid_client_name(event.client)
            || event.table_id <= 0 || event.table_id > tables_count) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}


bool process_event_line(const std::string& line, Event& event, int tables_count, const std::string& prev_time) 
{
    std::istringstream iss(line);
    if (!(iss >> event.time >> event.id) || !is_valid_time(event.time) || !is_time_non_decreasing(prev_time, event.time)) {
        return false;
    }

    if (!process_event_details(iss, event, tables_count)) {
        return false;
    }

    std::string extra;
    if (iss >> extra) {
        return false;
    }

    return true;
}


ComputerClub read_file(const string& filename)
{
    int tables_count;
    int hour_price;
    std::string start_time;
    std::string end_time;
    std::vector<Event> events;

    std::ifstream file("./tests/" + filename);
    if (!file) {
        cerr << "Error in the opening file: " << filename << endl;
        exit(1);
    }

    read_validate_positive_integer(file, tables_count);
    read_validate_times(file, start_time, end_time);
    read_validate_positive_integer(file, hour_price);

    string prev_time = "00:00";
    string line;
    while (getline(file, line)) {
        Event event;
        if (!process_event_line(line, event, tables_count, prev_time)) {
            cerr << line << endl;
            exit(0);
        }

        events.push_back(event);
        prev_time = event.time;
    }
    
    file.close();

    return ComputerClub(tables_count, hour_price, start_time, end_time, events);
}


void output_in_file(ComputerClub& club, const string& filename)
{
    ofstream out_file("./results/result_" + filename);
    if (out_file) {
        club.output_information(out_file);
    }
    out_file.close();
}


int main(int argc, char* argv[])
{
    if (argc == 1) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    try
    {
        ComputerClub club = read_file(argv[1]);
        club.process_events();
        club.output_information();
        output_in_file(club, argv[1]);
        
    }
    catch(const std::exception& e)
    {
        std::cerr << "" << e.what() << endl;
        return 1;
    }

    return 0;
}