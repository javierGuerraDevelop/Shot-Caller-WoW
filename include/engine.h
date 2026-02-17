#ifndef SHOTCALLERCPP_ENGINE_H
#define SHOTCALLERCPP_ENGINE_H

#include <algorithm>
#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "parser.h"

namespace ch = std::chrono;

struct AbilityState {
    int id;
    ch::seconds cooldown;
    ch::time_point<ch::system_clock> on_cooldown_until{};

    AbilityState(int id, ch::seconds cooldown) : id{id}, cooldown{cooldown} {}
    AbilityState() : id(0), cooldown(ch::seconds(0)) {}
};

struct EnemyAbility {
    int id;
    ch::milliseconds first_cast;
    ch::milliseconds cooldown;
    std::string callout;
    bool is_interruptable;

    EnemyAbility(int id, ch::milliseconds first_cast, ch::milliseconds cooldown,
                 std::string callout, bool is_interruptable)
        : id{id},
          first_cast{first_cast},
          cooldown{cooldown},
          callout{std::move(callout)},
          is_interruptable{is_interruptable} {}
};

struct Player {
    std::string id;
    std::string name;
    std::string p_class;
    AbilityState interrupt;
    std::map<int, AbilityState> crowd_control;
    bool is_alive{true};

    Player(std::string id, std::string name, std::string p_class, AbilityState interrupt,
           std::map<int, AbilityState> crowd_control)
        : id{std::move(id)},
          name{std::move(name)},
          p_class{std::move(p_class)},
          interrupt{interrupt},
          crowd_control{std::move(crowd_control)} {}

    Player() = default;
};

struct Enemy {
    std::string id;
    std::vector<EnemyAbility> spells;
    ch::time_point<ch::system_clock> combat_start_time;
    bool is_ccable;

    Enemy(std::string id, std::vector<EnemyAbility> spells,
          ch::time_point<ch::system_clock> combat_start_time, bool is_ccable)
        : id{std::move(id)},
          spells{std::move(spells)},
          combat_start_time{combat_start_time},
          is_ccable{is_ccable} {}
};

class ShotCallEngine {
   public:
    void handle_event(const CombatEvent& event);
    void handle_player_event(const CombatEvent& event);
    void handle_enemy_event(const CombatEvent& event);
    void handle_death(const CombatEvent& event);
    void identify_player(const CombatEvent& event);
    void identify_enemy(const CombatEvent& event);
    void generate_shotcalls(Enemy& enemy);
    void process_shotcalls();

    void set_shotcall_callback(
        std::function<void(const std::string&, const std::string&)> callback);

    static std::string get_player_class(int spell_id);

    const std::map<std::string, Player>& get_roster() const {
        return roster_;
    }
    const std::map<std::string, Enemy>& get_enemy_roster() const {
        return enemy_roster_;
    }
    size_t get_shotcall_queue_size() const {
        return shot_call_queue_.size();
    }

   private:
    std::string find_available_interrupter(const ch::time_point<ch::system_clock>& call_time);

    std::map<std::string, Player> roster_{};
    std::map<std::string, AbilityState> roster_interrupts_{};
    std::map<std::string, std::map<int, AbilityState>> roster_crowd_control_{};
    ch::time_point<ch::system_clock> current_time_{};
    std::map<std::string, Enemy> enemy_roster_{};
    std::list<std::tuple<std::string, std::string, ch::time_point<ch::system_clock>>>
        shot_call_queue_{};

    std::function<void(const std::string&, const std::string&)> shotcall_callback_;
};

#endif  // SHOTCALLERCPP_ENGINE_H
