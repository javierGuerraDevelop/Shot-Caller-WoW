#ifndef SHOTCALLERENGINEWOW_CONSTANTS_H
#define SHOTCALLERENGINEWOW_CONSTANTS_H

#include <array>
#include <chrono>
#include <string_view>

namespace Constants {

inline constexpr std::array<std::pair<int, std::string_view>, 37> kIdentifyingSpells{{
    // Death Knight
    {48743, "Death Knight"},   // Death's Advance (all specs)
    {49998, "Death Knight"},   // Death Strike (all specs)
    {47541, "Death Knight"},   // Death Coil (Unholy)
    // Demon Hunter
    {198013, "Demon Hunter"},  // Eye Beam (Havoc)
    {185123, "Demon Hunter"},  // Throw Glaive (all specs)
    {162794, "Demon Hunter"},  // Chaos Strike (Havoc)
    // Druid
    {1126, "Druid"},           // Mark of the Wild (all specs)
    {8921, "Druid"},           // Moonfire (all specs)
    {8936, "Druid"},           // Regrowth (all specs)
    // Evoker
    {364342, "Evoker"},        // Blessing of the Bronze (all specs)
    {361469, "Evoker"},        // Living Flame (all specs)
    // Hunter
    {5384, "Hunter"},          // Feign Death (all specs)
    {34026, "Hunter"},         // Kill Command (BM/Survival)
    {185358, "Hunter"},        // Arcane Shot (BM/MM)
    // Mage
    {1459, "Mage"},            // Arcane Intellect (all specs)
    {108853, "Mage"},          // Fire Blast (Fire/Frost)
    {116, "Mage"},             // Frostbolt (Frost)
    {133, "Mage"},             // Fireball (Fire)
    // Monk
    {116670, "Monk"},          // Vivify (all specs)
    {100780, "Monk"},          // Tiger Palm (all specs)
    {107428, "Monk"},          // Rising Sun Kick (WW/MW)
    // Paladin
    {19750, "Paladin"},        // Flash of Light (Holy)
    {275773, "Paladin"},       // Judgment (all specs)
    {35395, "Paladin"},        // Crusader Strike (Ret/Prot)
    // Priest
    {21562, "Priest"},         // Power Word: Fortitude (all specs)
    {585, "Priest"},           // Smite (all specs)
    {8092, "Priest"},          // Mind Blast (all specs)
    // Rogue
    {1784, "Rogue"},           // Stealth (all specs)
    {196819, "Rogue"},         // Eviscerate (all specs)
    // Shaman
    {10613, "Shaman"},         // Windfury Totem (Enhancement)
    {188196, "Shaman"},        // Lightning Bolt (all specs)
    {51505, "Shaman"},         // Lava Burst (Ele/Resto)
    // Warlock
    {686, "Warlock"},          // Shadow Bolt (Affliction/Demo)
    {29722, "Warlock"},        // Incinerate (Destruction)
    // Warrior
    {6673, "Warrior"},         // Battle Shout (all specs)
    {12294, "Warrior"},        // Mortal Strike (Arms)
    {23922, "Warrior"},        // Shield Slam (Protection)
}};

constexpr std::string_view GetClassFromIdentifyingSpells(int spell_id) {
    for (const auto& [id, class_name] : kIdentifyingSpells) {
        if (id == spell_id)
            return class_name;
    }

    return "";
}

inline constexpr std::array<std::string_view, 38> kIgnorableEvents{
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
    "SPELL_INSTAKILL",
    "SPELL_INTERRUPT",
    "SPELL_LEECH",
    "SPELL_MISSED",
    "SPELL_STOLEN",
    "SPELL_SUMMON",
    "SPELL_EMPOWER_START",
    "SPELL_EMPOWER_END",
    "SPELL_EMPOWER_INTERRUPT",
    "SPELL_PERIODIC_DAMAGE",
    "SPELL_PERIODIC_DRAIN",
    "SPELL_PERIODIC_ENERGIZE",
    "SPELL_PERIODIC_LEECH",
    "SPELL_PERIODIC_MISSED",
    "SPELL_BUILDING_DAMAGE",
    "SPELL_BUILDING_HEAL",
    "ENVIRONMENTAL_DAMAGE",
    "DAMAGE_SHIELD",
    "DAMAGE_SHIELD_MISSED",
    "DAMAGE_SPLIT",
    "PARTY_KILL",
    "UNIT_DIED",
    "UNIT_DESTROYED",
    "UNIT_DISSIPATES",
};

constexpr bool IsIgnorableEvent(std::string_view event) {
    for (const auto& item : kIgnorableEvents) {
        if (event == item) {
            return true;
        }
    }

    return false;
}

inline constexpr std::array<std::tuple<std::string_view, int, std::chrono::seconds>, 15>
    kInterruptData{{
        {"Death Knight", 47528, std::chrono::seconds{15}},   // Mind Freeze
        {"Demon Hunter", 183752, std::chrono::seconds{15}},  // Disrupt
        {"Druid", 78675, std::chrono::seconds{60}},          // Druid (Moonkin)
        {"Druid", 106839, std::chrono::seconds{15}},         // Skull Bash (Main kick)
        {"Evoker", 351338, std::chrono::seconds{20}},        // Quell
        {"Hunter", 187707, std::chrono::seconds{15}},        // Muzzle
        {"Mage", 2139, std::chrono::seconds{24}},            // Counterspell
        {"Monk", 116705, std::chrono::seconds{15}},          // Spear Hand Strike
        {"Paladin", 96231, std::chrono::seconds{15}},        // Rebuke
        {"Priest", 15487, std::chrono::seconds{45}},         // Silence
        {"Rogue", 1766, std::chrono::seconds{15}},           // Kick
        {"Shaman", 57994, std::chrono::seconds{12}},         // Wind Shear
        {"Warlock", 19647, std::chrono::seconds{24}},        // Spell Lock (Pet)
        {"Warrior", 6552, std::chrono::seconds{15}}          // Pummel
    }};

constexpr int GetInterruptId(std::string_view player_class) {
    for (const auto& [class_name, interrupt_id, interrupt_cd] : kInterruptData) {
        if (player_class == class_name) {
            return interrupt_id;
        }
    }

    return {};
}

constexpr std::chrono::seconds GetInterruptCd(std::string_view player_class) {
    for (const auto& [class_name, interrupt_id, interrupt_cd] : kInterruptData) {
        if (player_class == class_name) {
            return interrupt_cd;
        }
    }

    return {};
}

enum CcEnum : size_t {
    kClassName = 0,
    kSpellName = 1,
    kSpellId = 2,
    kSpellCd = 3,
};

inline constexpr std::array<
    std::tuple<std::string_view, std::string_view, int, std::chrono::seconds>, 31>
    kCrowdControlData{{{"Death Knight", "Blinding Sleet", 207127, std::chrono::seconds{60}},
                       {"Death Knight", "Gorefiend's Grasp", 207167, std::chrono::seconds{90}},
                       {"Demon Hunter", "Chaos Nova", 179057, std::chrono::seconds{60}},
                       {"Demon Hunter", "Sigil of Silence", 202138, std::chrono::seconds{90}},
                       {"Demon Hunter", "Sigil of Misery", 207684, std::chrono::seconds{90}},
                       {"Demon Hunter", "Sigil of Chains", 204598, std::chrono::seconds{90}},
                       {"Druid", "Mass Entanglement", 102359, std::chrono::seconds{30}},
                       {"Druid", "Ursol's Vortex", 102793, std::chrono::seconds{60}},
                       {"Druid", "Typhoon", 132469, std::chrono::seconds{30}},
                       {"Evoker", "Landslide", 371900, std::chrono::seconds{90}},
                       {"Evoker", "Deep Breath", 358269, std::chrono::seconds{120}},
                       {"Evoker", "Tail Swipe", 368725, std::chrono::seconds{90}},
                       {"Hunter", "Binding Shot", 109248, std::chrono::seconds{45}},
                       {"Mage", "Frost Nova", 122, std::chrono::seconds{30}},
                       {"Mage", "Dragon's Breath", 31661, std::chrono::seconds{45}},
                       {"Mage", "Ring of Frost", 113724, std::chrono::seconds{45}},
                       {"Mage", "Blast Wave", 157981, std::chrono::seconds{30}},
                       {"Monk", "Leg Sweep", 119381, std::chrono::seconds{60}},
                       {"Monk", "Ring of Peace", 116844, std::chrono::seconds{60}},
                       {"Paladin", "Blinding Light", 105421, std::chrono::seconds{90}},
                       {"Priest", "Psychic Scream", 8122, std::chrono::seconds{60}},
                       {"Shaman", "Capacitor Totem", 192058, std::chrono::seconds{60}},
                       {"Shaman", "Earthgrab Totem", 51485, std::chrono::seconds{30}},
                       {"Shaman", "Thunderstorm", 51490, std::chrono::seconds{45}},
                       {"Shaman", "Sundering", 197214, std::chrono::seconds{40}},
                       {"Warlock", "Howl of Terror", 5484, std::chrono::seconds{45}},
                       {"Warlock", "Shadowfury", 30283, std::chrono::seconds{60}},
                       {"Warrior", "Intimidating Shout", 5246, std::chrono::seconds{90}},
                       {"Warrior", "Shockwave", 46968, std::chrono::seconds{40}}}};

constexpr std::chrono::seconds GetCcCd(int id) {
    for (const auto& [class_name, spell_name, spell_id, spell_cd] : kCrowdControlData) {
        if (id == spell_id) {
            return spell_cd;
        }
    }

    return {};
}

constexpr std::string_view GetCcName(int id) {
    for (const auto& [class_name, spell_name, spell_id, spell_cd] : kCrowdControlData) {
        if (id == spell_id) {
            return spell_name;
        }
    }

    return {};
}

inline constexpr std::array<int, 5> kBattleRezIds{{10609, 376999, 20707, 61999, 407133}};

constexpr bool IsBattleRez(int spell_id) {
    for (const auto& id : kBattleRezIds) {
        if (id == spell_id)
            return true;
    }
    return false;
}

struct EnemySpellEntry {
    std::string_view enemy_id;
    int spell_id;
    long long first_cast_ms;
    long long cooldown_ms;
    std::string_view callout;
    bool is_interruptable;
    bool is_ccable;
};

inline constexpr std::array<EnemySpellEntry, 54> kEnemyData{{
    // Eco-dome
    {"245092", 1215850, 20000, 37000, "AoE", false, true},
    {"234883", 1221152, 6500, 18200, "AoE", false, true},
    {"242631", 1235368, 6900, 15800, "Tank Frontal", false, true},
    {"236995", 1226111, 15000, 20600, "Ejection", false, true},
    {"234957", 1221483, 15000, 20600, "Dispel", false, true},
    {"234962", 1221679, 6000, 13300, "Leap", false, true},
    // Tazavesh
    {"180567", 357827, 5000, 17000, "Leap", false, true},
    {"246285", 1240912, 14300, 23000, "Buster", false, true},
    {"246285", 1240821, 8000, 23000, "Spread", false, true},
    {"178165", 355429, 11300, 23000, "AOE", false, true},
    {"178141", 355132, 9700, 27900, "Fish sticks", false, true},
    {"180429", 357238, 13600, 26700, "Pulsar", false, true},
    {"179386", 368661, 8300, 14500, "Toss", false, true},
    {"177716", 351119, 8000, 18200, "Tee Pee", true, true},
    {"177816", 355915, 7300, 17000, "Dispel", false, true},
    {"180431", 357260, 13300, 21800, "Unstable Rift", true, true},
    // Halls of Atonement
    {"164557", 326409, 8900, 23000, "AOE", false, true},
    {"167607", 1235326, 15900, 32800, "Stop casting", false, true},
    {"164562", 326450, 15300, 24200, "Loyal Beast", true, true},
    {"165414", 325876, 9700, 24200, "Dispel", false, true},
    // Floodgate
    {"230748", 465827, 6800, 19400, "Warp blood", false, true},
    {"231014", 465120, 8300, 17000, "Loaderbots spinning", false, true},
    // Dawnbreaker
    {"214761", 432448, 8300, 23000, "Seed", false, true},
    {"214761", 431364, 3300, 10900, "Ray", false, true},
    {"210966", 451107, 4900, 20600, "Cocoon", false, true},
    {"228540", 431309, 12400, 23000, "Curse", false, true},
    {"213892", 431309, 12400, 23000, "Curse", false, true},
    {"211261", 451102, 14300, 27800, "Aoe", false, true},
    {"211261", 451119, 8300, 12100, "Dot", false, true},
    {"211262", 451119, 3900, 12100, "Dot", false, true},
    {"211263", 451119, 4900, 12100, "Dot", false, true},
    {"211263", 450854, 12100, 24300, "Orb", false, true},
    // Ara-kara
    {"216293", 434793, 4000, 16900, "AoE Barrage", true, true},
    {"217531", 434802, 9600, 20800, "Fear", true, true},
    {"218324", 438877, 12100, 21900, "AoE", false, true},
    {"216338", 1241693, 6000, 30300, "AoE", false, true},
    {"223253", 448248, 4800, 20600, "Volley", true, true},
    {"216364", 433841, 5800, 19000, "Volley", true, true},
    // Priory of the Sacred Flame
    {"206696", 427609, 20400, 23000, "Stop casting", false, true},
    {"206696", 427621, 3800, 15700, "Impale bleed", false, true},
    {"221760", 444743, 9500, 24300, "Volley", true, true},
    {"212826", 448485, 5900, 12100, "Tank Buster", false, true},
    {"212826", 448492, 14700, 15700, "AoE", false, true},
    {"212831", 427897, 10800, 18200, "AoE", false, true},
    {"239833", 424431, 26100, 37600, "AoE", false, true},
    {"206704", 448791, 15500, 21700, "AoE", false, true},
    {"206699", 446776, 7000, 15800, "Leap bleed", false, true},
    // Cinderbrew Meadery
    {"214697", 463206, 8100, 18100, "Knock", true, true},    // Tenderize
    {"210269", 463218, 8500, 24200, "DoT", true, true},      // Volatile Keg
    {"223423", 448619, 9100, 30300, "Charge", true, true},   // Reckless Delivery
    {"220946", 442995, 10300, 23000, "AoE", true, true},     // Swarming Surprise
    {"222964", 441434, 8700, 23000, "Batch", false, true},   // Failed Batch
    {"220141", 440687, 5900, 25400, "Volley", true, true},   // Honey Volley
    {"218671", 437956, 10500, 18200, "Dispel", true, true},  // Erupting Inferno
}};

constexpr bool IsTrackedEnemy(std::string_view enemy_id) {
    for (const auto& entry : kEnemyData) {
        if (entry.enemy_id == enemy_id)
            return true;
    }
    return false;
}

constexpr bool IsEnemyCcable(std::string_view enemy_id) {
    for (const auto& entry : kEnemyData) {
        if (entry.enemy_id == enemy_id)
            return entry.is_ccable;
    }
    return false;
}

}  // namespace Constants

#endif  // SHOTCALLERENGINEWOW_CONSTANTS_H
