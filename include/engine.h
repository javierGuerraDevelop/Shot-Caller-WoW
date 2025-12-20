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
using std::function;
using std::list;
using std::map;
using std::move;
using std::string;
using std::tuple;
using std::vector;

struct AbilityState
{
        int id;
        ch::seconds cooldown;
        ch::time_point<ch::system_clock> on_cooldown_until{};

        AbilityState(int id, ch::seconds cooldown) : id{ id }, cooldown{ cooldown } {}
        AbilityState() : id(0), cooldown(ch::seconds(0)) {}
};

struct EnemyAbility
{
        int id;
        ch::milliseconds first_cast;
        ch::milliseconds cooldown;
        string callout;
        bool is_interruptable;

        EnemyAbility(int id, ch::milliseconds first_cast, ch::milliseconds cooldown, string callout,
                     bool is_interruptable)
            : id{ id },
              first_cast{ first_cast },
              cooldown{ cooldown },
              callout{ move(callout) },
              is_interruptable{ is_interruptable }
        {
        }
};

struct Player
{
        string id;
        string name;
        string p_class;
        AbilityState interrupt;
        map<int, AbilityState> crowd_control;
        bool is_alive{ true };

        Player(string id, string name, string p_class, AbilityState interrupt,
               map<int, AbilityState> crowd_control)
            : id{ move(id) },
              name{ move(name) },
              p_class{ move(p_class) },
              interrupt{ interrupt },
              crowd_control{ move(crowd_control) }
        {
        }

        Player() = default;
};

struct Enemy
{
        string id;
        vector<EnemyAbility> spells;
        ch::time_point<ch::system_clock> combat_start_time;
        bool is_ccable;

        Enemy(string id, vector<EnemyAbility> spells,
              ch::time_point<ch::system_clock> combat_start_time, bool is_ccable)
            : id{ move(id) },
              spells{ move(spells) },
              combat_start_time{ combat_start_time },
              is_ccable{ is_ccable }
        {
        }
};

class ShotCallEngine
{
     public:
        void handle_event(const CombatEvent& event);
        void handle_player_event(const CombatEvent& event);
        void handle_enemy_event(const CombatEvent& event);
        void handle_death(const CombatEvent& event);
        void identify_player(const CombatEvent& event);
        void identify_enemy(const CombatEvent& event);
        void generate_shotcalls(Enemy& enemy);
        void process_shotcalls();

        void set_shotcall_callback(function<void(const string&, const string&)> callback);

        static string get_player_class(int spell_id);

        const map<string, Player>& get_roster() const
        {
                return roster_;
        }
        const map<string, Enemy>& get_enemy_roster() const
        {
                return enemy_roster_;
        }
        size_t get_shotcall_queue_size() const
        {
                return shot_call_queue_.size();
        }

     private:
        string find_available_interrupter(const ch::time_point<ch::system_clock>& call_time);

        map<string, Player> roster_{};
        map<string, AbilityState> roster_interrupts_{};
        map<string, map<int, AbilityState>> roster_crowd_control_{};
        ch::time_point<ch::system_clock> current_time_{};
        map<string, Enemy> enemy_roster_{};
        list<tuple<string, string, ch::time_point<ch::system_clock>>> shot_call_queue_{};

        function<void(const string&, const string&)> shotcall_callback_;
};

#endif  // SHOTCALLERCPP_ENGINE_H
