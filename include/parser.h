#ifndef SHOTCALLERCPP_PARSER_H
#define SHOTCALLERCPP_PARSER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ch = std::chrono;
using std::string;

struct CombatEvent
{
        ch::system_clock::time_point time_stamp;
        string event_type;
        string name;
        string source_id;
        string target_id;
        string source_raid_flag;
        string spell_name;
        int spell_id;
};

CombatEvent parse_line(const string& string);

#endif  // SHOTCALLERCPP_PARSER_H
