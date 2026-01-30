#ifndef SHOTCALLERCPP_STRUCTS_H
#define SHOTCALLERCPP_STRUCTS_H

#include <chrono>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "engine.h"

namespace ch = std::chrono;

namespace SpellDb {
extern const std::unordered_set<int> battle_rez_ids;
extern const std::map<std::string, Enemy> enemy_db;
extern const std::map<int, std::string> identifying_spells;
extern const std::unordered_set<std::string> ignorable_events;
extern const std::unordered_set<int> interrupt_ids;
extern const std::map<std::string, int> interrupt_map;
extern const std::map<int, ch::seconds> interrupt_cooldown_map;
extern const std::unordered_set<int> crowd_control_ids;
extern const std::map<std::string, std::vector<AbilityState>> crowd_control_map;
extern const std::map<int, ch::seconds> crowd_control_cooldown_map;
}  // namespace SpellDb

#endif  // SHOTCALLERCPP_STRUCTS_H
