#include "parser.h"

namespace ch = std::chrono;
using std::get_time;
using std::getline;
using std::mktime;
using std::stoi;
using std::string;
using std::stringstream;
using std::tm;
using std::vector;

ch::time_point<ch::system_clock> parse_timestamp(const string& timestamp)
{
        size_t time_zone_pos = timestamp.find_last_of("+-");
        if (time_zone_pos == string::npos)
                return {};

        string main_part = timestamp.substr(0, time_zone_pos);
        int timezone_offset_hours = stoi(timestamp.substr(time_zone_pos));

        size_t millisecond_pos = main_part.find('.');
        if (millisecond_pos == string::npos)
                return {};

        string date_time_part = timestamp.substr(0, millisecond_pos);
        int milliseconds = stoi(main_part.substr(millisecond_pos + 1));

        tm tm{};
        stringstream stream(date_time_part);
        stream >> get_time(&tm, "%m/%d/%Y/%H/%M/%S");

        time_t time = mktime(&tm);
        auto time_point = ch::system_clock::from_time_t(time);
        time_point += ch::milliseconds(milliseconds);
        time_point -= ch::hours(timezone_offset_hours);

        return time_point;
}

CombatEvent parse_line(const string& line)
{
        if (line.empty())
                return CombatEvent{};

        string part;
        vector<string> parts;
        stringstream stream(line);
        CombatEvent Combat_Event;

        while (getline(stream, part, ','))
                parts.push_back(part);

        if (parts.size() < 10)
                return {};

        size_t space_pos = parts[0].find("  ");
        if (space_pos != string::npos) {
                string timestamp_str = parts[0].substr(0, space_pos);
                Combat_Event.time_stamp = parse_timestamp(timestamp_str);
                Combat_Event.event_type = parts[0].substr(space_pos + 2);
        }

        stringstream name_stream(parts[2]);
        string player_name;
        getline(name_stream, player_name, '-');

        Combat_Event.name = player_name;
        Combat_Event.source_id = parts[1];
        Combat_Event.source_raid_flag = parts[3];
        Combat_Event.target_id = parts[5];
        Combat_Event.spell_name = parts[10];

        return Combat_Event;
}
