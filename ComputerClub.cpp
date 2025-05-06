#include "ComputerClub.h"


using namespace std;


ComputerClub::ComputerClub(int t_count, int h_price, std::string s_time, 
    std::string e_time, std::vector<Event> ev)
{
    tables_count = t_count;
    hour_price = h_price;
    start_time = s_time;
    end_time = e_time;
    for (auto e : ev)
        events.push_back(e);
    for (int i = 1; i <= t_count; i++)
        tables[i] = Table();
}


void ComputerClub::process_events()
{
    int n = events.size();
    Event event;
    for (int i = 0; i < n; i++) {
        event = events[i];
        switch (event.id)
        {
        case CLIENT_ARRIVED: {
            handle_client_arrived(event);
            break;
        }
        case CLIENT_SAT: {
            handle_client_sat(event);
            break;
        }
        case CLIENT_WAITING: {
            handle_client_waiting(event);
            break;
        }
        case CLIENT_LEFT: {
            handle_client_left(event);
            break;
        }
        }
    }
    handle_client_auto_left();
    calculate_income();

    stable_sort(events.begin(), events.end(), [this](const Event& a, const Event& b) {
        if (a.id == 11 && b.id == 11 && a.time == this->end_time) {
            return a.client < b.client;
        }
        return time_to_minutes(a.time) < time_to_minutes(b.time);
    });
}


void ComputerClub::output_information()
{
    output_information(cout);
}


void ComputerClub::output_information(std::ostream &out_file)
{
    output(out_file);
}


void ComputerClub::output(std::ostream& stream) 
{

    stream << start_time << endl;
    for (const auto& e : events) {
        if (e.id == 1 || e.id == 3 || e.id == 4 || e.id == 11) {
            stream << format("{} {} {}", e.time, e.id, e.client) << endl;
        } else if (e.id == 2 || e.id == 12) {
            stream << format("{} {} {} {}", e.time, e.id, e.client, e.table_id) << endl;
        } else if (e.id == 13) {
            stream << format("{} {} {}", e.time, e.id, e.error) << endl;
        }
    }

    stream << end_time << endl;
    for (int id = 1; id <= tables_count; id++) {
        stream << format("{} {} {}", id, tables[id].income, tables[id].operating_time) << endl;
    }
}


void ComputerClub::handle_client_arrived(Event& e)
{
    int time = time_to_minutes(e.time);
    if (time < time_to_minutes(start_time) || time >= time_to_minutes(end_time)) {
        events.push_back(Event(ERROR, e.time, e.client, "NotOpenYet"));
        return;
    }

    auto [it, inserted] = clients.try_emplace(e.client, Client(true));
    if (!inserted) {
        if (it->second.presence) {
            events.push_back(Event(ERROR, e.time, e.client, "YouShallNotPass"));
        } else {
            clients[e.client].presence = true;
        }
    }
}


void ComputerClub::handle_client_sat(Event &e)
{
    auto cl = clients.find(e.client);
    if (cl == clients.end() || !cl->second.presence) {
        events.push_back(Event(ERROR, e.time, e.client, "ClientUnknown"));
        return;
    }

    Table &new_table = tables[e.table_id];
    if (!new_table.current_client.empty()) {
        events.push_back(Event(ERROR, e.time, e.client, "PlaceIsBusy"));
        return;
    }

    int prev_table_id = cl->second.table_id;
    if (prev_table_id != -1) {
        Table &old_table = tables[prev_table_id];
        old_table.current_client.clear();

        Session &last_ses = old_table.sessions.back();
        last_ses.end_time = e.time;
    }

    new_table.current_client = e.client;
    cl->second.table_id = e.table_id;

    Session s;
    s.client = e.client;
    s.start_time = e.time;
    new_table.sessions.push_back(s);
}


void ComputerClub::handle_client_waiting(Event &e)
{
    auto cl = clients.find(e.client);
    if (cl == clients.end() || !cl->second.presence) {
        events.push_back(Event(ERROR, e.time, e.client, "ClientUnknown"));
        return;
    }

    if (are_tables_free()) {
        events.push_back(Event(ERROR, e.time, e.client, "ICanWaitNoLonger"));
        return;
    }

    if ((clients_queue.size() + 1) > tables_count) {
        events.push_back(Event(CLIENT_AUTO_LEFT, e.time, e.client));
        return;
    }
    clients_queue.push(e.client);
}


void ComputerClub::handle_client_left(Event &e)
{
    auto cl = clients.find(e.client);
    if (cl == clients.end() || !cl->second.presence) {
        events.push_back(Event(ERROR, e.time, e.client, "ClientUnknown"));
        return;
    }

    Client &old_client = cl->second;
    int table_id = old_client.table_id;

    if (table_id == -1) {
        old_client.presence = false;
        return;
    }

    Table &table = tables[table_id];
    Session &last_ses = table.sessions.back();
    last_ses.end_time = e.time;
    old_client.table_id = -1;
    old_client.presence = false;
    table.current_client.clear();

    if (!clients_queue.empty()) {
        string new_client = clients_queue.front();
        table.current_client = new_client;
        clients[new_client].table_id = table_id;

        clients_queue.pop();
        events.push_back(Event(CLIENT_SAT_FROM_QUEUE, e.time, new_client, table_id));

        Session s;
        s.client = new_client;
        s.start_time = e.time;
        table.sessions.push_back(s);
    }
}


void ComputerClub::handle_client_auto_left()
{
    for (auto& [id, table] : tables) {
        if (!table.current_client.empty()) {
            string client_name = table.current_client;
            table.current_client.clear();

            Session &last_ses = table.sessions.back();
            last_ses.end_time = end_time;

            clients[client_name].presence = false;
            clients[client_name].table_id = -1;

            events.push_back(Event(CLIENT_AUTO_LEFT, end_time, client_name));
        }
    }

    while (!clients_queue.empty()) {
        string client_name = clients_queue.front();
        clients_queue.pop();

        clients[client_name].presence = false;
        clients[client_name].table_id = -1;

        events.push_back(Event(CLIENT_AUTO_LEFT, end_time, client_name));
    }


    for (auto& [client_name, client] : clients) {
        if (client.presence && client.table_id == -1) {
            client.presence = false;
            events.push_back(Event(CLIENT_AUTO_LEFT, end_time, client_name));
        }
    }
}


bool ComputerClub::are_tables_free()
{
    for (auto& [id, table]: tables) {
        if (table.current_client.empty()) {
            return true;
        }
    }
    return false;
}


void ComputerClub::calculate_income()
{
    for (auto &[id, table] : tables) {
        int minutes = 0;
        int income  = 0;
        for (auto &ses : table.sessions) {
            if (ses.end_time.empty()) {
                ses.end_time = end_time;
            }
            int duration = time_to_minutes(ses.end_time) - time_to_minutes(ses.start_time);
            minutes += duration;

            int hours = (duration + 59) / 60;
            ses.income = hours * hour_price;
            income += ses.income;
        }
        table.income = income;

        int h = minutes / 60;
        int m = minutes % 60;
        table.operating_time = 
            (h < 10 ? "0" : "") + to_string(h) + ":" + 
            (m < 10 ? "0" : "") + to_string(m);
    }
}