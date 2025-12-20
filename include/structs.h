#ifndef SHOTCALLERCPP_STRUCTS_H
#define SHOTCALLERCPP_STRUCTS_H

#include <chrono>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "engine.h"

namespace ch = std::chrono;
using std::map;
using std::string;
using std::unordered_set;
using std::vector;

extern const unordered_set<int> battle_rez_ids;
extern const map<string, Enemy> enemy_db;
extern const map<int, string> identifying_spells;
extern const unordered_set<string> ignorable_events;
extern const unordered_set<int> interrupt_ids;
extern const map<string, int> interrupts_ids;
extern const map<int, ch::seconds> interrupt_cooldown_m;
extern const unordered_set<int> crowd_control_ids;
extern const map<string, vector<AbilityState>> crowd_control_m;
extern const map<int, ch::seconds> crowd_control_cooldown_m;

#endif  // SHOTCALLERCPP_STRUCTS_H
