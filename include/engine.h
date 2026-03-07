// Core shotcall engine. Tracks party members, enemies, and their ability
// cooldowns. When an enemy is identified, generates a time-sorted queue of
// upcoming shotcalls. A background thread dispatches callouts at the right
// time, assigning available interrupters or CC users to each call.

#ifndef SHOTCALLERCPP_ENGINE_H
#define SHOTCALLERCPP_ENGINE_H

#include <algorithm>
#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "parser.h"

namespace ch = std::chrono;

// Tracks a player ability's cooldown state.
struct AbilityState {
    int id;
    ch::seconds cooldown{};
    ch::time_point<ch::system_clock> on_cooldown_until{};

    AbilityState(int id, ch::seconds cooldown) : id{ id }, cooldown{ cooldown } {}
    AbilityState() : id{ 0 }, cooldown{ ch::seconds{ 0 } } {}
};

// Describes an enemy's ability with its cast timing and callout text.
struct EnemyAbility {
    int id;
    ch::milliseconds first_cast;
    ch::milliseconds cooldown;
    std::string callout;
    bool is_interruptable;

    EnemyAbility(int id, ch::milliseconds first_cast, ch::milliseconds cooldown,
                 std::string callout, bool is_interruptable)
        : id{ id },
          first_cast{ first_cast },
          cooldown{ cooldown },
          callout{ std::move(callout) },
          is_interruptable{ is_interruptable } {}
};

struct Player {
    std::string id;
    std::string name;
    std::string p_class;
    AbilityState interrupt;
    std::map<int, AbilityState> crowd_control;
    bool is_alive = true;

    Player(std::string id, std::string name, std::string p_class, AbilityState interrupt,
           std::map<int, AbilityState> crowd_control)
        : id{ std::move(id) },
          name{ std::move(name) },
          p_class{ std::move(p_class) },
          interrupt{ interrupt },
          crowd_control{ std::move(crowd_control) } {}

    Player() = default;
};

struct Enemy {
    std::string id;
    std::vector<EnemyAbility> spells;
    ch::time_point<ch::system_clock> combat_start_time;
    bool is_ccable;

    Enemy(std::string id, std::vector<EnemyAbility> spells,
          ch::time_point<ch::system_clock> combat_start_time, bool is_ccable)
        : id{ std::move(id) },
          spells{ std::move(spells) },
          combat_start_time{ combat_start_time },
          is_ccable{ is_ccable } {}
};

class ShotCallEngine {
   public:
    // Routes incoming combat events to the appropriate handler.
    void handle_event(const CombatEvent& event);
    void handle_player_event(const CombatEvent& event);
    void handle_enemy_event(const CombatEvent& event);
    void handle_death(const CombatEvent& event);
    // Auto-detect player class or enemy type from combat event spells.
    void identify_player(const CombatEvent& event);
    void identify_enemy(const CombatEvent& event);
    // Pre-computes all shotcalls for an enemy over a 5-minute window.
    void generate_shotcalls(Enemy& enemy);
    // Runs on a background thread; dispatches queued shotcalls at their scheduled time.
    void process_shotcalls();

    void set_shotcall_callback(
        std::function<void(const std::string&, const std::string&)> callback);

   private:
    // Returns the name of a living player whose interrupt/CC is off cooldown
    // at call_time, or a fallback message if none available.
    std::string find_available_interrupter(const ch::time_point<ch::system_clock>& call_time);
    std::string find_available_ccer(const ch::time_point<ch::system_clock>& call_time);
    std::function<void(const std::string&, const std::string&)> shotcall_callback_;

    std::mutex mtx_;  // Guards all mutable state below
    std::map<std::string, Player> roster_;
    std::map<std::string, AbilityState> roster_interrupts_;
    std::map<std::string, std::map<int, AbilityState>> roster_crowd_control_;
    std::map<std::string, Enemy> enemy_roster_;
    // Queue entries: (is_interruptable, enemy_id, callout_text, scheduled_time)
    std::list<std::tuple<bool, std::string, std::string, ch::time_point<ch::system_clock>>>
        shot_call_queue_;
};

#endif  // SHOTCALLERCPP_ENGINE_H
