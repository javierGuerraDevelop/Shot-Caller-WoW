#include "engine.h"

#include "spell_db.h"

namespace ch = std::chrono;
using std::function;
using std::get;
using std::make_tuple;
using std::map;
using std::string;
using std::tuple;

bool is_battle_rez(int spell_id);
bool is_ignorable_event(const string& event_type);
bool is_interrupt(int spell_id);
ch::seconds get_interrupt_cooldown(int spell_id);
int get_interrupt_id(const string& p_class);
bool is_crowd_control(int spell_id);
ch::seconds get_crowd_control_cooldown(int spell_id);
map<int, AbilityState> get_crowd_control_m(const string& p_class);

bool is_cast_by_party_member(const CombatEvent& event)
{
        // 0x511 and 0x1511 are the bits for enemies
        return event.source_raid_flag == "0x511" || event.source_raid_flag == "0x1511";
}

void ShotCallEngine::handle_event(const CombatEvent& event)
{
        if (is_cast_by_party_member(event))
                handle_player_event(event);
        else
                handle_enemy_event(event);
}

void ShotCallEngine::handle_death(const CombatEvent& event)
{
        auto player_iter = roster_.find(event.target_id);
        if (player_iter != roster_.end()) {
                player_iter->second.is_alive = false;
                return;
        }

        auto enemy_iter = enemy_roster_.find(event.source_id);
        if (enemy_iter != enemy_roster_.end()) {
                string enemy_id = enemy_iter->second.id;
                shot_call_queue_.remove_if([&enemy_id](const auto& shotcall) {
                        return get<0>(shotcall) == enemy_id;
                });
                enemy_roster_.erase(enemy_iter);
        }
}

void ShotCallEngine::handle_player_event(const CombatEvent& event)
{
        if (is_ignorable_event(event.event_type))
                return;

        auto player_iter = roster_.find(event.source_id);
        if (player_iter == roster_.end()) {
                identify_player(event);
                player_iter = roster_.find(event.source_id);
                if (player_iter == roster_.end())
                        return;
        }

        auto& player = player_iter->second;
        if (!player.is_alive)
                return;

        auto time_now = event.time_stamp;
        if (is_battle_rez(event.spell_id) && event.event_type == "SPELL_CAST_SUCCESS") {
                auto target_iter = roster_.find(event.target_id);
                if (target_iter != roster_.end())
                        target_iter->second.is_alive = true;
                return;
        } else if (is_interrupt(event.spell_id)) {
                auto& player_interrupt = roster_interrupts_[player_iter->first];
                auto player_interrupt_cd = player_interrupt.cooldown;
                player_interrupt.on_cooldown_until = time_now + player_interrupt_cd;
        } else if (is_crowd_control(event.spell_id)) {
                auto& player_crowd_control_map = roster_crowd_control_[player_iter->first];
                auto cc_iter = player_crowd_control_map.find(event.spell_id);
                if (cc_iter != player_crowd_control_map.end()) {
                        auto crowd_control_cd = cc_iter->second.cooldown;
                        cc_iter->second.on_cooldown_until = time_now + crowd_control_cd;
                }
        }
}

void ShotCallEngine::handle_enemy_event(const CombatEvent& event)
{
        auto iter = enemy_roster_.find(event.source_id);
        if (iter == enemy_roster_.end())
                identify_enemy(event);
}

void ShotCallEngine::generate_shotcalls(Enemy& enemy)
{
        for (size_t spell_idx = 0; spell_idx < enemy.spells.size(); ++spell_idx) {
                const auto& spell = enemy.spells[spell_idx];
                long long cd_ms = spell.cooldown.count();
                if (cd_ms <= 0)
                        continue;

                long long five_minutes_ms = 300000;
                auto iterations = (five_minutes_ms / cd_ms) + 1;
                for (size_t i = 0; i < iterations; ++i) {
                        ch::milliseconds duration;
                        if (i == 0)
                                duration = spell.first_cast;
                        else
                                duration = spell.first_cast + ch::milliseconds(i * cd_ms);

                        tuple<string, string, ch::time_point<ch::system_clock>> shotcall{
                                make_tuple(enemy.id, spell.callout,
                                           (enemy.combat_start_time + duration))
                        };
                        shot_call_queue_.push_back(shotcall);
                }
        }
        shot_call_queue_.sort([](const auto& a, const auto& b) {
                return get<2>(a) < get<2>(b);
        });
}

string ShotCallEngine::find_available_interrupter(const ch::time_point<ch::system_clock>& call_time)
{
        for (const auto& [player_id, player] : roster_) {
                if (!player.is_alive)
                        continue;

                auto interrupt_iter = roster_interrupts_.find(player_id);
                if (interrupt_iter == roster_interrupts_.end())
                        continue;

                if (interrupt_iter->second.on_cooldown_until <= call_time)
                        return player.name;
        }
        return "this one is going off";
}

void ShotCallEngine::set_shotcall_callback(function<void(const string&, const string&)> callback)
{
        shotcall_callback_ = callback;
}

void ShotCallEngine::process_shotcalls()
{
        while (!shot_call_queue_.empty()) {
                auto& front_call = shot_call_queue_.front();
                const string& enemy_id = get<0>(front_call);
                const string& callout = get<1>(front_call);
                const auto& call_time = get<2>(front_call);
                auto time_now = ch::system_clock::now();
                auto time_until_call = ch::duration_cast<ch::milliseconds>(call_time - time_now);
                if (time_until_call.count() < 0) {
                        shot_call_queue_.pop_front();
                        continue;
                }
                if (time_until_call.count() > 1000) {
                        std::this_thread::sleep_for(
                            ch::milliseconds(time_until_call.count() - 1000));
                        continue;
                }

                auto enemy_iter = enemy_roster_.find(enemy_id);
                if (enemy_iter == enemy_roster_.end()) {
                        shot_call_queue_.pop_front();
                        continue;
                }

                string available_player = find_available_interrupter(call_time);
                if (shotcall_callback_) {
                        string full_callout = callout;
                        if (!available_player.empty())
                                full_callout = available_player + " " + callout;
                        shotcall_callback_(enemy_id, full_callout);
                }
                shot_call_queue_.pop_front();
                std::this_thread::sleep_for(ch::milliseconds(100));
        }
}

void ShotCallEngine::identify_player(const CombatEvent& event)
{
        string p_class = get_player_class(event.spell_id);
        if (p_class.empty())
                return;

        int interrupt_id = get_interrupt_id(p_class);
        ch::seconds interrupt_cooldown = get_interrupt_cooldown(interrupt_id);
        AbilityState interrupt(interrupt_id, interrupt_cooldown);
        map<int, AbilityState> crowd_control_m = get_crowd_control_m(p_class);
        Player new_player(event.source_id, event.name, p_class, interrupt, crowd_control_m);
        roster_.emplace(event.source_id, new_player);
        roster_interrupts_.emplace(event.source_id, interrupt);
        roster_crowd_control_.emplace(event.source_id, crowd_control_m);
}

bool is_battle_rez(int spell_id)
{
        return SpellDb::battle_rez_ids.find(spell_id) != SpellDb::battle_rez_ids.end();
}

void ShotCallEngine::identify_enemy(const CombatEvent& event)
{
        auto iter = SpellDb::enemy_db.find(event.name);
        if (iter == SpellDb::enemy_db.end())
                return;

        auto new_enemy = iter->second;
        new_enemy.combat_start_time = event.time_stamp;
        enemy_roster_.emplace(event.source_id, new_enemy);
        ShotCallEngine::generate_shotcalls(new_enemy);
}

string ShotCallEngine::get_player_class(int spell_id)
{
        auto iter = SpellDb::identifying_spells.find(spell_id);
        if (iter != SpellDb::identifying_spells.end())
                return iter->second;
        else
                return "";
}

bool is_ignorable_event(const string& event_type)
{
        return SpellDb::ignorable_events.find(event_type) != SpellDb::ignorable_events.end();
}

bool is_interrupt(int spell_id)
{
        return SpellDb::interrupt_ids.find(spell_id) != SpellDb::interrupt_ids.end();
}

int get_interrupt_id(const string& p_class)
{
        auto iter = SpellDb::interrupt_map.find(p_class);
        if (iter != SpellDb::interrupt_map.end())
                return iter->second;
        else
                return 0;
}

ch::seconds get_interrupt_cooldown(int spell_id)
{
        using namespace std::chrono_literals;
        auto iter = SpellDb::interrupt_cooldown_map.find(spell_id);
        if (iter != SpellDb::interrupt_cooldown_map.end())
                return iter->second;
        else
                return 0s;
}

bool is_crowd_control(int spell_id)
{
        return SpellDb::crowd_control_ids.find(spell_id) != SpellDb::crowd_control_ids.end();
}

map<int, AbilityState> get_crowd_control_m(const string& p_class)
{
        auto iter = SpellDb::crowd_control_map.find(p_class);
        if (iter == SpellDb::crowd_control_map.end())
                return {};

        map<int, AbilityState> new_map;
        const auto& crowd_control_vec = iter->second;
        for (const auto& ability : crowd_control_vec)
                new_map[ability.id] = ability;

        return new_map;
}

ch::seconds get_crowd_control_cooldown(int spell_id)
{
        using namespace std::chrono_literals;
        auto iter = SpellDb::crowd_control_cooldown_map.find(spell_id);
        if (iter != SpellDb::crowd_control_cooldown_map.end())
                return iter->second;
        else
                return 0s;
}
