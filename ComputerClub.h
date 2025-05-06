#ifndef COMPUTERCLUB_H
#define COMPUTERCLUB_H

#include <vector>
#include <string>
#include <queue>
#include <iostream>
#include <format>
#include <fstream>
#include <unordered_map>

#include "utils.h"


enum EventID: int
{
    CLIENT_ARRIVED = 1,
    CLIENT_SAT = 2,
    CLIENT_WAITING = 3,
    CLIENT_LEFT = 4,
    CLIENT_AUTO_LEFT = 11,
    CLIENT_SAT_FROM_QUEUE = 12,
    ERROR = 13
};


struct Event
{
    int id;
    std::string time;
    std::string client;
    
    int table_id = -1;
    std::string error = "";

    Event() = default;
    Event(int id, std::string time, std::string client):
    id(id), time(time), client(client) {};

    Event(int id, std::string time, std::string client, std::string error):
    id(id), time(time), client(client), error(error) {};

    Event(int id, std::string time, std::string client, int table_id):
    id(id), time(time), client(client), table_id(table_id) {};
};


struct Session {
    std::string client;
    std::string start_time;
    std::string end_time;
    int income = 0;;

    Session() = default;
};


struct Table
{
    std::string current_client = "";
    std::string operating_time = "";
    int income = 0;
    std::vector<Session> sessions;

    Table() = default;
};

struct Client
{
    bool presence = false;
    int table_id = -1;

    Client() = default;
    Client(bool presence): presence(presence) {};
};



class ComputerClub
{
private:
    int tables_count;
    int hour_price;
    std::string start_time;
    std::string end_time;
    std::vector<Event> events;
    std::unordered_map<int, Table> tables;
    std::unordered_map<std::string, Client> clients;
    std::queue<std::string> clients_queue;


    void handle_client_arrived(Event& event);
    void handle_client_sat(Event& event);
    void handle_client_waiting(Event& event);
    void handle_client_left(Event& event);
    void handle_client_auto_left();

    bool are_tables_free();
    void calculate_income();
    void output(std::ostream& stream);

public:
    ComputerClub() = default;
    ComputerClub(int t_count, int h_price, std::string s_time, 
        std::string e_time, std::vector<Event> ev);

    ~ComputerClub() = default;

    void process_events();
    void output_information();
    void output_information(std::ostream& stream);
};


#endif