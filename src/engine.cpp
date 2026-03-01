#include "engine.h"

#include "constants.h"

namespace ch = std::chrono;

std::map<int, AbilityState> get_crowd_control_m(const std::string& p_class);

bool is_cast_by_party_member(const CombatEvent& event) {
    // WoW unit flags: 0x1 = MINE, 0x2 = PARTY, 0x400 = TYPE_PLAYER
    // A party member player has TYPE_PLAYER set and either MINE or PARTY affiliation
    unsigned long flag = 0;
    try {
        flag = std::stoul(event.source_raid_flag, nullptr, 16);
    } catch (...) {
        return false;
    }
    bool is_player = (flag & 0x400) != 0;
    bool is_party_or_mine = (flag & 0x3) != 0;
    return is_player && is_party_or_mine;
}

void ShotCallEngine::handle_event(const CombatEvent& event) {
    if (is_cast_by_party_member(event)) {
        handle_player_event(event);
    } else {
        handle_enemy_event(event);
    }
}

void ShotCallEngine::handle_death(const CombatEvent& event) {
    auto player_iter = roster_.find(event.target_id);
    if (player_iter != roster_.end()) {
        player_iter->second.is_alive = false;
        return;
    }

    auto enemy_iter = enemy_roster_.find(event.source_id);
    if (enemy_iter != enemy_roster_.end()) {
        std::string enemy_id = enemy_iter->second.id;
        shot_call_queue_.remove_if([&enemy_id](const auto& shotcall) {
            return std::get<0>(shotcall) == enemy_id;
        });
        enemy_roster_.erase(enemy_iter);
    }
}

void ShotCallEngine::handle_player_event(const CombatEvent& event) {
    auto player_iter = roster_.find(event.source_id);
    if (player_iter == roster_.end()) {
        identify_player(event);
        player_iter = roster_.find(event.source_id);
    }

    if (Constants::is_ignorable_event(event.event_type)) {
        return;
    }

    if (Constants::is_battle_rez(event.spell_id) && event.event_type == "SPELL_CAST_SUCCESS") {
        if (auto it = roster_.find(event.target_id); it != roster_.end()) {
            it->second.is_alive = true;
        }
    } else if (Constants::is_interrupt(event.spell_id)) {
        auto& player_interrupt = roster_interrupts_[player_iter->first];
        auto player_interrupt_cd = player_interrupt.cooldown;
        player_interrupt.on_cooldown_until = event.time_stamp + player_interrupt_cd;
    } else if (Constants::is_crowd_control(event.spell_id)) {
        auto& player_crowd_control_map = roster_crowd_control_[player_iter->first];
        if (auto it = player_crowd_control_map.find(event.spell_id);
            it != player_crowd_control_map.end()) {
            auto crowd_control_cd = it->second.cooldown;
            it->second.on_cooldown_until = event.time_stamp + crowd_control_cd;
        }
    }
}

void ShotCallEngine::handle_enemy_event(const CombatEvent& event) {
    if (auto it = enemy_roster_.find(event.source_id); it != enemy_roster_.end()) {
        identify_enemy(event);
    }
}

void ShotCallEngine::generate_shotcalls(Enemy& enemy) {
    for (size_t i = 0; i < enemy.spells.size(); ++i) {
        const auto& spell = enemy.spells[i];
        long long cd_ms = spell.cooldown.count();
        if (cd_ms <= 0) {
            continue;
        }

        long long five_minutes_ms = 300000;
        auto iterations = (five_minutes_ms / cd_ms) + 1;
        for (size_t i = 0; i < iterations; ++i) {
            ch::milliseconds duration{};
            if (i == 0) {
                duration = spell.first_cast;
            } else {
                duration = spell.first_cast + ch::milliseconds(i * cd_ms);
            }

            std::tuple<std::string, std::string, ch::time_point<ch::system_clock>> shotcall{
                std::make_tuple(enemy.id, spell.callout, (enemy.combat_start_time + duration))
            };
            shot_call_queue_.push_back(shotcall);
        }
    }
    shot_call_queue_.sort([](const auto& a, const auto& b) {
        return std::get<2>(a) < std::get<2>(b);
    });
}

std::string ShotCallEngine::find_available_interrupter(
    const ch::time_point<ch::system_clock>& call_time) {
    for (const auto& [player_id, player] : roster_) {
        if (!player.is_alive) {
            continue;
        }

        auto interrupt_iter = roster_interrupts_.find(player_id);
        if (interrupt_iter == roster_interrupts_.end()) {
            continue;
        }

        if (interrupt_iter->second.on_cooldown_until <= call_time) {
            return player.name;
        }
    }
    return "this one is going off";
}

void ShotCallEngine::set_shotcall_callback(
    std::function<void(const std::string&, const std::string&)> callback) {
    shotcall_callback_ = callback;
}

void ShotCallEngine::process_shotcalls() {
    while (true) {
        if (shot_call_queue_.empty()) {
            std::this_thread::sleep_for(ch::milliseconds(250));
            continue;
        }
        auto& front_call = shot_call_queue_.front();
        const std::string& enemy_id = std::get<0>(front_call);
        const std::string& callout = std::get<1>(front_call);
        const auto& call_time = std::get<2>(front_call);
        auto time_now = ch::system_clock::now();
        auto time_until_call = ch::duration_cast<ch::milliseconds>(call_time - time_now);
        if (time_until_call.count() < 0) {
            shot_call_queue_.pop_front();
            continue;
        }
        if (time_until_call.count() > 1000) {
            std::this_thread::sleep_for(ch::milliseconds(time_until_call.count() - 1000));
            continue;
        }

        if (auto it = enemy_roster_.find(enemy_id); it != enemy_roster_.end()) {
            shot_call_queue_.pop_front();
            continue;
        }

        std::string available_player = find_available_interrupter(call_time);
        if (shotcall_callback_) {
            std::string full_callout = callout;
            if (!available_player.empty()) {
                full_callout = available_player + " " + callout;
            }
            shotcall_callback_(enemy_id, full_callout);
        }
        shot_call_queue_.pop_front();
        std::this_thread::sleep_for(ch::milliseconds(100));
    }
}

void ShotCallEngine::identify_player(const CombatEvent& event) {
    std::string p_class{ Constants::get_class_from_identifying_spells(event.spell_id) };
    if (p_class.empty()) {
        return;
    }

    int interrupt_id = Constants::get_interrupt_id(p_class);
    ch::seconds interrupt_cooldown = Constants::get_interrupt_cd(p_class);
    AbilityState interrupt{ interrupt_id, interrupt_cooldown };
    std::map<int, AbilityState> crowd_control_m = get_crowd_control_m(p_class);
    Player new_player{ event.source_id, event.name, p_class, interrupt, crowd_control_m };
    roster_.emplace(event.source_id, new_player);
    roster_interrupts_.emplace(event.source_id, interrupt);
    roster_crowd_control_.emplace(event.source_id, crowd_control_m);
}

bool is_battle_rez(int spell_id) {
    return Constants::is_battle_rez(spell_id);
}

void ShotCallEngine::identify_enemy(const CombatEvent& event) {
    if (event.npc_id.empty() || !Constants::is_tracked_enemy(event.npc_id)) {
        return;
    }

    std::vector<EnemyAbility> spells;
    for (const auto& entry : Constants::enemy_data) {
        if (entry.enemy_id == event.npc_id) {
            spells.emplace_back(entry.spell_id, ch::milliseconds(entry.first_cast_ms),
                                ch::milliseconds(entry.cooldown_ms), std::string(entry.callout),
                                entry.is_interruptable);
        }
    }
    bool is_ccable = Constants::is_enemy_ccable(event.npc_id);
    Enemy new_enemy{ event.source_id, spells, event.time_stamp, is_ccable };
    enemy_roster_.emplace(event.source_id, new_enemy);
    ShotCallEngine::generate_shotcalls(new_enemy);
}

std::map<int, AbilityState> get_crowd_control_m(const std::string& p_class) {
    std::map<int, AbilityState> new_map{};
    for (const auto& [class_name, spell_name, id, cd] : Constants::crowd_control_data) {
        if (class_name == p_class) {
            new_map[id] = AbilityState{ id, cd };
        }
    }
    return new_map;
}
