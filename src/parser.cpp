#include "parser.h"

std::chrono::time_point<std::chrono::system_clock> parse_timestamp(const std::string& timestamp)
{
    size_t time_zone_pos = timestamp.find_last_of("+-");
    if (time_zone_pos == std::string::npos) {
        return {};
    }

    std::string main_part = timestamp.substr(0, time_zone_pos);
    int timezone_offset_hours = std::stoi(timestamp.substr(time_zone_pos));

    size_t millisecond_pos = main_part.find('.');
    if (millisecond_pos == std::string::npos) {
        return {};
    }

    std::string date_time_part = timestamp.substr(0, millisecond_pos);
    int milliseconds = std::stoi(main_part.substr(millisecond_pos + 1));

    std::tm tm{};
    std::stringstream stream(date_time_part);
    stream >> std::get_time(&tm, "%m/%d/%Y/%H/%M/%S");

    std::time_t time = std::mktime(&tm);
    auto time_point = std::chrono::system_clock::from_time_t(time);
    time_point += std::chrono::milliseconds(milliseconds);
    time_point -= std::chrono::hours(timezone_offset_hours);

    return time_point;
}

CombatEvent parse_line(const std::string& line)
{
    if (line.empty())
        return CombatEvent{};

    std::string part;
    std::vector<std::string> parts;
    std::stringstream stream(line);
    CombatEvent Combat_Event;

    while (std::getline(stream, part, ',')) {
        parts.push_back(part);
    }

    if (parts.size() < 10)
        return {};

    size_t space_pos = parts[0].find("  ");
    if (space_pos != std::string::npos) {
        std::string timestamp_str = parts[0].substr(0, space_pos);
        Combat_Event.time_stamp = parse_timestamp(timestamp_str);
        Combat_Event.event_type = parts[0].substr(space_pos + 2);
    }

    std::stringstream name_stream(parts[2]);
    std::string player_name;
    std::getline(name_stream, player_name, '-');

    // switch(event_type) with hash function (take that from line_reader)
    Combat_Event.name = player_name;
    Combat_Event.source_id = parts[1];
    Combat_Event.source_raid_flag = parts[3];
    Combat_Event.target_id = parts[5];
    Combat_Event.spell_name = parts[10];

    return Combat_Event;
}
