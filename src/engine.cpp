#include "engine.h"

#include "parser.h"

bool is_battle_rez(int spell_id);
bool is_ignorable_event(const std::string& event_type);
bool is_interrupt(int spell_id);
std::chrono::seconds get_interrupt_cooldown(int spell_id);
int get_interrupt_id(const std::string& p_class);
bool is_crowd_control(int spell_id);
std::chrono::seconds get_crowd_control_cooldown(int spell_id);
std::map<int, AbilityState> get_crowd_control_m(const std::string& p_class);

constexpr unsigned int hash_str(const std::string_view s) {
    unsigned int hashed = 0;

    for (char c : s)
        hashed = (hashed * 31) + c;

    return hashed;
}

bool is_cast_by_party_member(const CombatEvent& event) {
    return event.source_raid_flag == "0x511" || event.source_raid_flag == "0x1511";
}

void ShotCallEngine::handle_event(const CombatEvent& event) {
    if (is_cast_by_party_member(event)) {
        handle_player_event(event);
    } else {
        handle_enemy_event(event);
    }
}

void ShotCallEngine::handle_death(const CombatEvent& event) {
    // remove shot calls placed in queue for dead enemy
}

void ShotCallEngine::handle_player_event(const CombatEvent& event) {
    if (is_ignorable_event(event.event_type))
        return;

    auto player_iter = roster_.find(event.source_id);
    if (player_iter == roster_.end()) {
        identify_player(event);
        player_iter = roster_.find(event.source_id);
    }

    auto player = player_iter->second;
    if (!player.is_alive)
        return;

    auto time_now = event.time_stamp;

    if (is_battle_rez(event.spell_id) && event.event_type == "SPELL_CAST_SUCCESS") {
        roster_[event.target_id].is_alive = true;
        return;
    } else if (is_interrupt(event.spell_id)) {
        auto& player_interrupt = roster_interrupts_[player_iter->first];
        auto player_interrupt_cd = player_interrupt.cooldown;
        player_interrupt.on_cooldown_until = time_now + player_interrupt_cd;
    } else if (is_crowd_control(event.spell_id)) {
        auto& player_crowd_control_m = roster_crowd_control_[player_iter->first];
        auto crowd_control_cd = player_crowd_control_m[event.spell_id].cooldown;
        player_crowd_control_m[event.spell_id].on_cooldown_until = time_now + crowd_control_cd;
    }
}

void ShotCallEngine::handle_enemy_event(const CombatEvent& event) {
    auto iter = enemy_roster_.find(event.source_id);
    if (iter == enemy_roster_.end()) {
        identify_enemy(event);
        return;
    }
}

void ShotCallEngine::generate_shotcalls(Enemy& enemy) {
    // this will generate shot calls for the next 5 minutes for each individual enemy
    for (auto spell : enemy.spells) {
        long long cd = spell.cooldown.count();
        cd /= 1000;
        auto iterations = (300 / cd) + 1;

        for (size_t i = 1; i <= iterations; ++i) {
            auto duration = std::chrono::milliseconds(i * spell.cooldown);
            std::tuple<std::string, std::string, std::chrono::time_point<std::chrono::system_clock>> shotcall{
                std::make_tuple(enemy.id, enemy.spells[i].callout, (enemy.combat_start_time + duration * i))
            };
            shot_call_queue_.push_back(shotcall);
        }
    }
}

void ShotCallEngine::identify_player(const CombatEvent& event) {
    std::string p_class = get_player_class(event.spell_id);
    int interrupt_id = get_interrupt_id(p_class);
    std::chrono::seconds interrupt_cooldown = get_interrupt_cooldown(interrupt_id);
    AbilityState interrupt(interrupt_id, interrupt_cooldown);

    std::map<int, AbilityState> crowd_control_m = get_crowd_control_m(p_class);

    Player new_player(event.source_id, event.name, p_class, interrupt, crowd_control_m);
    new_player.id = event.source_id;
    new_player.name = event.name;
    new_player.p_class = get_player_class(event.spell_id);
    new_player.interrupt = interrupt;
    new_player.crowd_control = {};
    new_player.is_alive = true;
}

bool is_battle_rez(int spell_id) {
    static const std::unordered_set<int> battle_rez_ids = {
        10609, 376999, 20707, 61999, 407133,
    };

    if (battle_rez_ids.find(spell_id) != battle_rez_ids.end()) {
        return true;
    } else {
        return false;
    }
}
void ShotCallEngine::identify_enemy(const CombatEvent& event) {
    static const std::map<std::string, Enemy> enemy_db{
        // Eco-dome----------------------------------------------------------------------------------------------------
        { "245092", Enemy("245092",
                          std::vector<EnemyAbility>{ EnemyAbility(1215850, std::chrono::milliseconds(20000),
                                                                  std::chrono::milliseconds(37000), "AoE", false) },
                          {}, true) },
        { "234883", Enemy("234883",
                          std::vector<EnemyAbility>{ EnemyAbility(1221152, std::chrono::milliseconds(6500),
                                                                  std::chrono::milliseconds(18200), "AoE", false) },
                          {}, true) },
        { "242631",
          Enemy("242631",
                std::vector<EnemyAbility>{ EnemyAbility(1235368, std::chrono::milliseconds(6900),
                                                        std::chrono::milliseconds(15800), "Tank Frontal", false) },
                {}, true) },
        { "236995",
          Enemy("236995",
                std::vector<EnemyAbility>{ EnemyAbility(1226111, std::chrono::milliseconds(15000),
                                                        std::chrono::milliseconds(20600), "Ejection", false) },
                {}, true) },
        { "234957", Enemy("234957",
                          std::vector<EnemyAbility>{ EnemyAbility(1221483, std::chrono::milliseconds(15000),
                                                                  std::chrono::milliseconds(20600), "Dispel", false) },
                          {}, true) },
        { "234962", Enemy("234962",
                          std::vector<EnemyAbility>{ EnemyAbility(1221679, std::chrono::milliseconds(6000),
                                                                  std::chrono::milliseconds(13300), "Leap", false) },
                          {}, true) },
        // Tazavesh----------------------------------------------------------------------------------------------------
        { "180567", Enemy("180567",
                          std::vector<EnemyAbility>{ EnemyAbility(357827, std::chrono::milliseconds(5000),
                                                                  std::chrono::milliseconds(17000), "Leap", false) },
                          {}, true) },
        { "246285", Enemy("246285",
                          std::vector<EnemyAbility>{ EnemyAbility(1240912, std::chrono::milliseconds(14300),
                                                                  std::chrono::milliseconds(23000), "Buster", false),
                                                     EnemyAbility(1240821, std::chrono::milliseconds(8000),
                                                                  std::chrono::milliseconds(23000), "Spread", false) },
                          {}, true) },
        { "178165", Enemy("178165",
                          std::vector<EnemyAbility>{ EnemyAbility(355429, std::chrono::milliseconds(11300),
                                                                  std::chrono::milliseconds(23000), "AOE", false) },
                          {}, true) },
        { "178141",
          Enemy("178141",
                std::vector<EnemyAbility>{ EnemyAbility(355132, std::chrono::milliseconds(9700),
                                                        std::chrono::milliseconds(27900), "Fish sticks", false) },
                {}, true) },
        { "180429", Enemy("180429",
                          std::vector<EnemyAbility>{ EnemyAbility(357238, std::chrono::milliseconds(13600),
                                                                  std::chrono::milliseconds(26700), "Pulsar", false) },
                          {}, true) },
        { "179386", Enemy("179386",
                          std::vector<EnemyAbility>{ EnemyAbility(368661, std::chrono::milliseconds(8300),
                                                                  std::chrono::milliseconds(14500), "Toss", false) },
                          {}, true) },
        { "177716", Enemy("177716",
                          std::vector<EnemyAbility>{ EnemyAbility(351119, std::chrono::milliseconds(8000),
                                                                  std::chrono::milliseconds(18200), "Tee Pee", true) },
                          {}, true) },
        { "177816", Enemy("177816",
                          std::vector<EnemyAbility>{ EnemyAbility(355915, std::chrono::milliseconds(7300),
                                                                  std::chrono::milliseconds(17000), "Dispel", false) },
                          {}, true) },
        { "180431",
          Enemy("180431",
                std::vector<EnemyAbility>{ EnemyAbility(357260, std::chrono::milliseconds(13300),
                                                        std::chrono::milliseconds(21800), "Unstable Rift", true) },
                {}, true) },

        // Halls of Attonement-----------------------------------------------------------------------------------------
        { "164557", Enemy("164557",
                          std::vector<EnemyAbility>{ EnemyAbility(326409, std::chrono::milliseconds(8900),
                                                                  std::chrono::milliseconds(23000), "AOE", false) },
                          {}, true) },
        { "167607",
          Enemy("167607",
                std::vector<EnemyAbility>{ EnemyAbility(1235326, std::chrono::milliseconds(15900),
                                                        std::chrono::milliseconds(32800), "Stop casting", false) },
                {}, true) },
        { "164562",
          Enemy("164562",
                std::vector<EnemyAbility>{ EnemyAbility(326450, std::chrono::milliseconds(15300),
                                                        std::chrono::milliseconds(24200), "Loyal Beast", true) },
                {}, true) },
        { "165414", Enemy("165414",
                          std::vector<EnemyAbility>{ EnemyAbility(325876, std::chrono::milliseconds(9700),
                                                                  std::chrono::milliseconds(24200), "Dispel", false) },
                          {}, true) },

        // Floodgate---------------------------------------------------------------------------------------------------
        { "230748",
          Enemy("230748",
                std::vector<EnemyAbility>{ EnemyAbility(465827, std::chrono::milliseconds(6800),
                                                        std::chrono::milliseconds(19400), "Warp blood", false) },
                {}, true) },
        { "231014", Enemy("231014",
                          std::vector<EnemyAbility>{ EnemyAbility(465120, std::chrono::milliseconds(8300),
                                                                  std::chrono::milliseconds(17000),
                                                                  "Loaderbots spinning", false) },
                          {}, true) },

        // Dawnbreaker-------------------------------------------------------------------------------------------------
        { "214761", Enemy("214761",
                          std::vector<EnemyAbility>{ EnemyAbility(432448, std::chrono::milliseconds(8300),
                                                                  std::chrono::milliseconds(23000), "Seed", false),
                                                     EnemyAbility(431364, std::chrono::milliseconds(3300),
                                                                  std::chrono::milliseconds(10900), "Ray", false) },
                          {}, true) },
        { "210966", Enemy("210966",
                          std::vector<EnemyAbility>{ EnemyAbility(451107, std::chrono::milliseconds(4900),
                                                                  std::chrono::milliseconds(20600), "Cocoon", false) },
                          {}, true) },
        { "228540", Enemy("228540",
                          std::vector<EnemyAbility>{ EnemyAbility(431309, std::chrono::milliseconds(12400),
                                                                  std::chrono::milliseconds(23000), "Curse", false) },
                          {}, true) },
        { "213892", Enemy("213892",
                          std::vector<EnemyAbility>{ EnemyAbility(431309, std::chrono::milliseconds(12400),
                                                                  std::chrono::milliseconds(23000), "Curse", false) },
                          {}, true) },
        { "211261", Enemy("211261",
                          std::vector<EnemyAbility>{ EnemyAbility(451102, std::chrono::milliseconds(14300),
                                                                  std::chrono::milliseconds(27800), "Aoe", false),
                                                     EnemyAbility(451119, std::chrono::milliseconds(8300),
                                                                  std::chrono::milliseconds(12100), "Dot", false) },
                          {}, true) },
        { "211262", Enemy("211262",
                          std::vector<EnemyAbility>{ EnemyAbility(451119, std::chrono::milliseconds(3900),
                                                                  std::chrono::milliseconds(12100), "Dot", false) },
                          {}, true) },
        { "211263", Enemy("211263",
                          std::vector<EnemyAbility>{ EnemyAbility(451119, std::chrono::milliseconds(4900),
                                                                  std::chrono::milliseconds(12100), "Dot", false),
                                                     EnemyAbility(450854, std::chrono::milliseconds(12100),
                                                                  std::chrono::milliseconds(24300), "Orb", false) },
                          {}, true) },

        // Ara-kara----------------------------------------------------------------------------------------------------
        { "216293",
          Enemy("216293",
                std::vector<EnemyAbility>{ EnemyAbility(434793, std::chrono::milliseconds(4000),
                                                        std::chrono::milliseconds(16900), "AoE Barrage", true) },
                {}, true) },
        { "217531", Enemy("217531",
                          std::vector<EnemyAbility>{ EnemyAbility(434802, std::chrono::milliseconds(9600),
                                                                  std::chrono::milliseconds(20800), "Fear", true) },
                          {}, true) },
        { "218324", Enemy("218324",
                          std::vector<EnemyAbility>{ EnemyAbility(438877, std::chrono::milliseconds(12100),
                                                                  std::chrono::milliseconds(21900), "AoE", false) },
                          {}, true) },
        { "216338", Enemy("216338",
                          std::vector<EnemyAbility>{ EnemyAbility(1241693, std::chrono::milliseconds(6000),
                                                                  std::chrono::milliseconds(30300), "AoE", false) },
                          {}, true) },
        { "223253", Enemy("223253",
                          std::vector<EnemyAbility>{ EnemyAbility(448248, std::chrono::milliseconds(4800),
                                                                  std::chrono::milliseconds(20600), "Volley", true) },
                          {}, true) },
        { "216364", Enemy("216364",
                          std::vector<EnemyAbility>{ EnemyAbility(433841, std::chrono::milliseconds(5800),
                                                                  std::chrono::milliseconds(19000), "Volley", true) },
                          {}, true) },
        // Priory of the Sacred Flame----------------------------------------------------------------------------------
        { "206696",
          Enemy("206696",
                std::vector<EnemyAbility>{ EnemyAbility(427609, std::chrono::milliseconds(20400),
                                                        std::chrono::milliseconds(23000), "Stop casting", false),
                                           EnemyAbility(427621, std::chrono::milliseconds(3800),
                                                        std::chrono::milliseconds(15700), "Impale bleed", false) },
                {}, true) },
        { "221760", Enemy("221760",
                          std::vector<EnemyAbility>{ EnemyAbility(444743, std::chrono::milliseconds(9500),
                                                                  std::chrono::milliseconds(24300), "Volley", true) },
                          {}, true) },
        { "212826",
          Enemy("212826",
                std::vector<EnemyAbility>{ EnemyAbility(448485, std::chrono::milliseconds(5900),
                                                        std::chrono::milliseconds(12100), "Tank Buster", false),
                                           EnemyAbility(448492, std::chrono::milliseconds(14700),
                                                        std::chrono::milliseconds(15700), "AoE", false) },
                {}, true) },
        { "212831", Enemy("212831",
                          std::vector<EnemyAbility>{ EnemyAbility(427897, std::chrono::milliseconds(10800),
                                                                  std::chrono::milliseconds(18200), "AoE", false) },
                          {}, true) },
        { "239833", Enemy("239833",
                          std::vector<EnemyAbility>{ EnemyAbility(424431, std::chrono::milliseconds(26100),
                                                                  std::chrono::milliseconds(37600), "AoE", false) },
                          {}, true) },
        { "206704", Enemy("206704",
                          std::vector<EnemyAbility>{ EnemyAbility(448791, std::chrono::milliseconds(15500),
                                                                  std::chrono::milliseconds(21700), "AoE", false) },
                          {}, true) },
        { "206699",
          Enemy("206699",
                std::vector<EnemyAbility>{ EnemyAbility(446776, std::chrono::milliseconds(7000),
                                                        std::chrono::milliseconds(15800), "Leap bleed", false) },
                {}, true) },
    };

    auto iter = enemy_db.find(event.name);
    if (iter == enemy_db.end())
        return;

    auto new_enemy = iter->second;
    new_enemy.combat_start_time = event.time_stamp;
    enemy_roster_.emplace(event.source_id, new_enemy);
    ShotCallEngine::generate_shotcalls(new_enemy);
}

std::string ShotCallEngine::get_player_class(int spell_id) {
    static const std::map<int, std::string> identifying_spells = {
        { 48743, "Death Knight" },   // Death's Advance
        { 198013, "Demon Hunter" },  // Eye Beam
        { 1126, "Druid" },           // Mark of the Wild
        { 364342, "Evoker" },        // Blessing of the Bronze (Evoker buff)
        { 5384, "Hunter" },          // Feign Death
        { 1459, "Mage" },            // Arcane Intellect
        { 116670, "Monk" },          // Vivify
        { 19750, "Paladin" },        // Flash of Light
        { 21562, "Priest" },         // Power Word: Fortitude
        { 1784, "Rogue" },           // Stealth
        { 10613, "Shaman" },         // Windfury Totem
        { 6673, "Warrior" }          // Battle Shout
    };

    auto iter = identifying_spells.find(spell_id);
    if (iter != identifying_spells.end()) {
        return iter->second;
    } else {
        return "";
    }
}

bool is_ignorable_event(const std::string& event_type) {
    static const std::unordered_set<std::string> ignorable_events = {
        "SWING_DAMAGE",
        "SWING_MISSED",
        "RANGE_DAMAGE",
        "RANGE_MISSED",
        "SPELL_AURA_APPLIED",
        "SPELL_AURA_APPLIED_DOSE",
        "SPELL_AURA_BROKEN",
        "SPELL_AURA_BROKEN_SPELL",
        "SPELL_AURA_REFRESH",
        "SPELL_AURA_REMOVED",
        "SPELL_AURA_REMOVED_DOSE",
        "SPELL_CAST_FAILED",
        "SPELL_CAST_START",
        "SPELL_CREATE",
        "SPELL_DAMAGE",
        "SPELL_DISPEL_FAILED",
        "SPELL_HEAL",
        "SPELL_HEAL_ABSORBED",
        "SPELL_INSTAKILL",
        "SPELL_INTERRUPT",
        "SPELL_LEECH",
        "SPELL_MISSED",
        "SPELL_RESURRECT",
        "SPELL_STOLEN",
        "SPELL_SUMMON",
        "SPELL_EMPOWER_START",
        "SPELL_EMPOWER_END",
        "SPELL_EMPOWER_INTERRUPT",
        "SPELL_PERIODIC_DAMAGE",
        "SPELL_PERIODIC_DRAIN",
        "SPELL_PERIODIC_ENERGIZE",
        "SPELL_PERIODIC_HEAL",
        "SPELL_PERIODIC_LEECH",
        "SPELL_PERIODIC_MISSED",
        "SPELL_BUILDING_DAMAGE",
        "SPELL_BUILDING_HEAL",
        "ENVIRONMENTAL_DAMAGE",
        "DAMAGE_SHIELD",
        "DAMAGE_SHIELD_MISSED",
        "DAMAGE_SPLIT",
        "ENCHANT_APPLIED",
        "ENCHANT_REMOVED",
        "PARTY_KILL",
        "UNIT_DIED",
        "UNIT_DESTROYED",
        "UNIT_DISSIPATES",
        "DURABILITY_DAMAGE",
        "DURABILITY_DAMAGE_ALL",
    };

    if (ignorable_events.find(event_type) != ignorable_events.end()) {
        return true;
    } else {
        return false;
    }
}

bool is_interrupt(int spell_id) {
    static const std::unordered_set<int> interrupt_ids = {
        47528,   // Death Knight (Mind Freeze)
        183752,  // Demon Hunter (Disrupt)
        78675,   // Druid (Moonkin)
        106839,  // Druid (Skull Bash)
        351338,  // Evoker (Quell)
        187707,  // Hunter (Muzzle)
        2139,    // Mage (Counterspell)
        116705,  // Monk (Spear Hand Strike)
        96231,   // Paladin (Rebuke)
        15487,   // Priest (Silence)
        1766,    // Rogue (Kick)
        57994,   // Shaman (Wind Shear)
        19647,   // Warlock (Spell Lock)
        6552     // Warrior (Pummel)
    };

    if (interrupt_ids.find(spell_id) != interrupt_ids.end()) {
        return true;
    } else {
        return false;
    }
}

int get_interrupt_id(const std::string& p_class) {
    static const std::map<std::string, int> interrupts_ids = {
        { "Death Knight", 47528 },   // Mind Freeze
        { "Demon Hunter", 183752 },  // Disrupt
        { "Druid", 106839 },         // Skull Bash (Main kick)
        { "Evoker", 351338 },        // Quell
        { "Hunter", 187707 },        // Muzzle
        { "Mage", 2139 },            // Counterspell
        { "Monk", 116705 },          // Spear Hand Strike
        { "Paladin", 96231 },        // Rebuke
        { "Priest", 15487 },         // Silence
        { "Rogue", 1766 },           // Kick
        { "Shaman", 57994 },         // Wind Shear
        { "Warlock", 19647 },        // Spell Lock (Pet)
        { "Warrior", 6552 }          // Pummel
    };

    auto iter = interrupts_ids.find(p_class);
    if (iter == interrupts_ids.end()) {
        return iter->second;
    } else {
        return {};
    }
}

std::chrono::seconds get_interrupt_cooldown(int spell_id) {
    using namespace std::chrono_literals;
    static const std::map<int, std::chrono::seconds> interrupt_cooldown_m = {
        { 47528, 15s },   // Death Knight (Mind Freeze)
        { 183752, 15s },  // Demon Hunter (Disrupt)
        { 78675, 60s },   // Druid (Moonkin)
        { 106839, 15s },  // Druid (Skull Bash)
        { 351338, 20s },  // Evoker (Quell)
        { 187707, 15s },  // Hunter (Muzzle)
        { 2139, 24s },    // Mage (Counterspell)
        { 116705, 15s },  // Monk (Spear Hand Strike)
        { 96231, 15s },   // Paladin (Rebuke)
        { 15487, 45s },   // Priest (Silence)
        { 1766, 15s },    // Rogue (Kick)
        { 57994, 12s },   // Shaman (Wind Shear)
        { 19647, 24s },   // Warlock (Spell Lock)
        { 6552, 15s },    // Warrior (Pummel)
    };

    auto iter = interrupt_cooldown_m.find(spell_id);
    if (iter != interrupt_cooldown_m.end()) {
        return iter->second;
    } else {
        return 0s;
    }
}

bool is_crowd_control(int spell_id) {
    static const std::unordered_set<int> crowd_control_ids = {
        // Death Knight
        207127,  // Blinding Sleet
        207167,  // Gorefiend's Grasp
        // Demon Hunter
        179057,  // Chaos Nova
        202138,  // Sigil of Silence
        207684,  // Sigil of Misery
        204598,  // Sigil of Chains
        // Druid
        102359,  // Mass Entanglement
        102793,  // Ursol's Vortex
        132469,  // Typhoon
        // Evoker
        371900,  // Landslide
        358269,  // Deep Breath
        368725,  // Tail Swipe
        // Hunter
        109248,  // Binding Shot
        // Mage
        122,     // Frost Nova
        31661,   // Dragon's Breath
        113724,  // Ring of Frost
        157981,  // Blast Wave
        // Monk
        119381,  // Leg Sweep
        116844,  // Ring of Peace
        // Paladin
        105421,  // Blinding Light
        // Priest
        8122,  // Psychic Scream
        // Shaman
        192058,  // Capacitor Totem
        51485,   // Earthgrab Totem
        51490,   // Thunderstorm
        197214,  // Sundering
        // Warlock
        5484,   // Howl of Terror
        30283,  // Shadowfury
        1122,   // Infernal
        // Warrior
        5246,  // Intimidating Shout
        46968  // Shockwave
    };

    if (crowd_control_ids.find(spell_id) != crowd_control_ids.end()) {
        return true;
    } else {
        return false;
    }
}

std::map<int, AbilityState> get_crowd_control_m(const std::string& p_class) {
    using namespace std::chrono_literals;
    static const std::map<std::string, std::vector<AbilityState>> crowd_control_m = {
        { "Death Knight",
          {
              AbilityState(207127, 60s),  // Blinding Sleet
              AbilityState(207167, 90s),  // Gorefiend's Grasp
          } },
        { "Demon Hunter",
          {
              AbilityState(179057, 60s),  // Chaos Nova
              AbilityState(202138, 90s),  // Sigil of Silence
              AbilityState(207684, 90s),  // Sigil of Misery
              AbilityState(204598, 90s),  // Sigil of Chains
          } },
        { "Druid",
          {
              AbilityState(102359, 30s),  // Mass Entanglement
              AbilityState(102793, 60s),  // Ursol's Vortex
              AbilityState(132469, 30s),  // Typhoon
          } },
        { "Evoker",
          {
              AbilityState(371900, 90s),   // Landslide
              AbilityState(358269, 120s),  // Deep Breath
              AbilityState(368725, 90s),   // Tail Swipe (Dracthyr Racial)
          } },
        { "Hunter",
          {
              AbilityState(109248, 45s),  // Binding Shot
          } },
        { "Mage",
          {
              AbilityState(122, 30s),     // Frost Nova
              AbilityState(31661, 45s),   // Dragon's Breath
              AbilityState(113724, 45s),  // Ring of Frost
              AbilityState(157981, 30s),  // Blast Wave
          } },
        { "Monk",
          {
              AbilityState(119381, 60s),  // Leg Sweep
              AbilityState(116844, 60s),  // Ring of Peace
          } },
        { "Paladin",
          {
              AbilityState(105421, 90s),  // Blinding Light
          } },
        { "Priest",
          {
              AbilityState(8122, 60s),  // Psychic Scream
          } },
        { "Shaman",
          {
              AbilityState(192058, 60s),  // Capacitor Totem
              AbilityState(51485, 30s),   // Earthgrab Totem (Talent)
              AbilityState(51490, 45s),   // Thunderstorm
              AbilityState(197214, 40s),  // Sundering (Enhancement Talent)
          } },
        { "Warlock",
          {
              AbilityState(5484, 45s),   // Howl of Terror
              AbilityState(30283, 60s),  // Shadowfury
              AbilityState(1122, 180s),  // Infernal (Summon cooldown)
          } },
        { "Warrior",
          {
              AbilityState(5246, 90s),  // Intimidating Shout
              AbilityState(46968, 40s)  // Shockwave (Talent)
          } }
    };

    auto iter = crowd_control_m.find(p_class);
    if (iter != crowd_control_m.end()) {
        return {};
    }

    std::map<int, AbilityState> new_map;
    auto& crowd_control_vec = iter->second;

    for (const auto& ability : crowd_control_vec) {
        new_map[ability.id] = ability;
    }

    return new_map;
}

std::chrono::seconds get_crowd_control_cooldown(int spell_id) {
    using namespace std::chrono_literals;
    static const std::map<int, std::chrono::seconds> crowd_control_cooldown_m = {
        // Death Knight
        { 207127, 60s },  // Blinding Sleet
        { 207167, 90s },  // Gorefiend's Grasp
        // Demon Hunter
        { 179057, 60s },  // Chaos Nova
        { 202138, 90s },  // Sigil of Silence
        { 207684, 90s },  // Sigil of Misery
        { 204598, 90s },  // Sigil of Chains
        // Druid
        { 102359, 30s },  // Mass Entanglement
        { 102793, 60s },  // Ursol's Vortex
        { 132469, 30s },  // Typhoon
        // Evoker
        { 371900, 90s },   // Landslide
        { 358269, 120s },  // Deep Breath
        { 368725, 90s },   // Tail Swipe (Dracthyr Racial)
        // Hunter
        { 109248, 45s },  // Binding Shot
        // Mage
        { 122, 30s },     // Frost Nova
        { 31661, 45s },   // Dragon's Breath
        { 113724, 45s },  // Ring of Frost
        { 157981, 30s },  // Blast Wave
        // Monk
        { 119381, 60s },  // Leg Sweep
        { 116844, 60s },  // Ring of Peace
        // Paladin
        { 105421, 90s },  // Blinding Light
        // Priest
        { 8122, 60s },  // Psychic Scream
        // Shaman
        { 192058, 60s },  // Capacitor Totem
        { 51485, 30s },   // Earthgrab Totem (Talent)
        { 51490, 45s },   // Thunderstorm
        { 197214, 40s },  // Sundering (Enhancement Talent)
        // Warlock
        { 5484, 45s },   // Howl of Terror
        { 30283, 60s },  // Shadowfury
        { 1122, 180s },  // Infernal (Summon cooldown)
        // Warrior
        { 5246, 90s },  // Intimidating Shout
        { 46968, 40s }  // Shockwave (Talent)
    };

    auto iter = crowd_control_cooldown_m.find(spell_id);
    if (iter != crowd_control_cooldown_m.end()) {
        return iter->second;
    } else {
        return 0s;
    }
}

/*
 * log line is input to parser
 * parser outputs combat_event
 * handle event passes the combat_event to handle_player or handle_enemy
 * handlePlayer adds to roster until full, then updates cooldown
 *      -add check for death[]
 *      -add logic for brez
 * handleEnemy adds to enemy roster, queues up first cast as well as recast
 *      -add check to only add enemies that can cast
 *      -find optimal way to find enemies asap so combatStart is accurate
 * add the spellDB stuff from Go and the Lua stuff from causese
 * when a shotcall is 1 second before ready, check if mob is alive then send
 * when unit dies, remove enemy from roster and any queued callouts
 * find a way to see hey multiple enemies should be casting at the same
 * time, call kicks
 */

/*
 * identify_player should push_back roster interrupts and roster cc.
 */
