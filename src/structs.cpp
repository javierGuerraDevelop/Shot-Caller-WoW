#include "structs.h"

namespace ch = std::chrono;
using std::map;
using std::string;
using std::unordered_set;
using std::vector;

const unordered_set<int> battle_rez_ids = {
        10609, 376999, 20707, 61999, 407133,
};

const map<string, Enemy> enemy_db{
        // Eco-dome--------------------------------------------------------------------------------
        { "245092",
          Enemy("245092",
                vector<EnemyAbility>{ EnemyAbility(1215850, ch::milliseconds(20000),
                                                   ch::milliseconds(37000), "AoE", false) },
                {}, true) },
        { "234883",
          Enemy("234883",
                vector<EnemyAbility>{ EnemyAbility(1221152, ch::milliseconds(6500),
                                                   ch::milliseconds(18200), "AoE", false) },
                {}, true) },
        { "242631", Enemy("242631",
                          vector<EnemyAbility>{ EnemyAbility(1235368, ch::milliseconds(6900),
                                                             ch::milliseconds(15800),
                                                             "Tank Frontal", false) },
                          {}, true) },
        { "236995",
          Enemy("236995",
                vector<EnemyAbility>{ EnemyAbility(1226111, ch::milliseconds(15000),
                                                   ch::milliseconds(20600), "Ejection", false) },
                {}, true) },
        { "234957",
          Enemy("234957",
                vector<EnemyAbility>{ EnemyAbility(1221483, ch::milliseconds(15000),
                                                   ch::milliseconds(20600), "Dispel", false) },
                {}, true) },
        { "234962",
          Enemy("234962",
                vector<EnemyAbility>{ EnemyAbility(1221679, ch::milliseconds(6000),
                                                   ch::milliseconds(13300), "Leap", false) },
                {}, true) },
        // Tazavesh--------------------------------------------------------------------------------
        { "180567",
          Enemy("180567",
                vector<EnemyAbility>{ EnemyAbility(357827, ch::milliseconds(5000),
                                                   ch::milliseconds(17000), "Leap", false) },
                {}, true) },
        { "246285",
          Enemy("246285",
                vector<EnemyAbility>{ EnemyAbility(1240912, ch::milliseconds(14300),
                                                   ch::milliseconds(23000), "Buster", false),
                                      EnemyAbility(1240821, ch::milliseconds(8000),
                                                   ch::milliseconds(23000), "Spread", false) },
                {}, true) },
        { "178165",
          Enemy("178165",
                vector<EnemyAbility>{ EnemyAbility(355429, ch::milliseconds(11300),
                                                   ch::milliseconds(23000), "AOE", false) },
                {}, true) },
        { "178141",
          Enemy("178141",
                vector<EnemyAbility>{ EnemyAbility(355132, ch::milliseconds(9700),
                                                   ch::milliseconds(27900), "Fish sticks", false) },
                {}, true) },
        { "180429",
          Enemy("180429",
                vector<EnemyAbility>{ EnemyAbility(357238, ch::milliseconds(13600),
                                                   ch::milliseconds(26700), "Pulsar", false) },
                {}, true) },
        { "179386",
          Enemy("179386",
                vector<EnemyAbility>{ EnemyAbility(368661, ch::milliseconds(8300),
                                                   ch::milliseconds(14500), "Toss", false) },
                {}, true) },
        { "177716",
          Enemy("177716",
                vector<EnemyAbility>{ EnemyAbility(351119, ch::milliseconds(8000),
                                                   ch::milliseconds(18200), "Tee Pee", true) },
                {}, true) },
        { "177816",
          Enemy("177816",
                vector<EnemyAbility>{ EnemyAbility(355915, ch::milliseconds(7300),
                                                   ch::milliseconds(17000), "Dispel", false) },
                {}, true) },
        { "180431", Enemy("180431",
                          vector<EnemyAbility>{ EnemyAbility(357260, ch::milliseconds(13300),
                                                             ch::milliseconds(21800),
                                                             "Unstable Rift", true) },
                          {}, true) },

        // Halls of
        // Attonement------------------------------------------------------------------------------
        { "164557",
          Enemy("164557",
                vector<EnemyAbility>{ EnemyAbility(326409, ch::milliseconds(8900),
                                                   ch::milliseconds(23000), "AOE", false) },
                {}, true) },
        { "167607", Enemy("167607",
                          vector<EnemyAbility>{ EnemyAbility(1235326, ch::milliseconds(15900),
                                                             ch::milliseconds(32800),
                                                             "Stop casting", false) },
                          {}, true) },
        { "164562",
          Enemy("164562",
                vector<EnemyAbility>{ EnemyAbility(326450, ch::milliseconds(15300),
                                                   ch::milliseconds(24200), "Loyal Beast", true) },
                {}, true) },
        { "165414",
          Enemy("165414",
                vector<EnemyAbility>{ EnemyAbility(325876, ch::milliseconds(9700),
                                                   ch::milliseconds(24200), "Dispel", false) },
                {}, true) },

        // Floodgate-------------------------------------------------------------------------------
        { "230748",
          Enemy("230748",
                vector<EnemyAbility>{ EnemyAbility(465827, ch::milliseconds(6800),
                                                   ch::milliseconds(19400), "Warp blood", false) },
                {}, true) },
        { "231014", Enemy("231014",
                          vector<EnemyAbility>{ EnemyAbility(465120, ch::milliseconds(8300),
                                                             ch::milliseconds(17000),
                                                             "Loaderbots spinning", false) },
                          {}, true) },

        // Dawnbreaker-----------------------------------------------------------------------------
        { "214761",
          Enemy("214761",
                vector<EnemyAbility>{ EnemyAbility(432448, ch::milliseconds(8300),
                                                   ch::milliseconds(23000), "Seed", false),
                                      EnemyAbility(431364, ch::milliseconds(3300),
                                                   ch::milliseconds(10900), "Ray", false) },
                {}, true) },
        { "210966",
          Enemy("210966",
                vector<EnemyAbility>{ EnemyAbility(451107, ch::milliseconds(4900),
                                                   ch::milliseconds(20600), "Cocoon", false) },
                {}, true) },
        { "228540",
          Enemy("228540",
                vector<EnemyAbility>{ EnemyAbility(431309, ch::milliseconds(12400),
                                                   ch::milliseconds(23000), "Curse", false) },
                {}, true) },
        { "213892",
          Enemy("213892",
                vector<EnemyAbility>{ EnemyAbility(431309, ch::milliseconds(12400),
                                                   ch::milliseconds(23000), "Curse", false) },
                {}, true) },
        { "211261",
          Enemy("211261",
                vector<EnemyAbility>{ EnemyAbility(451102, ch::milliseconds(14300),
                                                   ch::milliseconds(27800), "Aoe", false),
                                      EnemyAbility(451119, ch::milliseconds(8300),
                                                   ch::milliseconds(12100), "Dot", false) },
                {}, true) },
        { "211262",
          Enemy("211262",
                vector<EnemyAbility>{ EnemyAbility(451119, ch::milliseconds(3900),
                                                   ch::milliseconds(12100), "Dot", false) },
                {}, true) },
        { "211263",
          Enemy("211263",
                vector<EnemyAbility>{ EnemyAbility(451119, ch::milliseconds(4900),
                                                   ch::milliseconds(12100), "Dot", false),
                                      EnemyAbility(450854, ch::milliseconds(12100),
                                                   ch::milliseconds(24300), "Orb", false) },
                {}, true) },

        // Ara-kara--------------------------------------------------------------------------------
        { "216293",
          Enemy("216293",
                vector<EnemyAbility>{ EnemyAbility(434793, ch::milliseconds(4000),
                                                   ch::milliseconds(16900), "AoE Barrage", true) },
                {}, true) },
        { "217531",
          Enemy("217531",
                vector<EnemyAbility>{ EnemyAbility(434802, ch::milliseconds(9600),
                                                   ch::milliseconds(20800), "Fear", true) },
                {}, true) },
        { "218324",
          Enemy("218324",
                vector<EnemyAbility>{ EnemyAbility(438877, ch::milliseconds(12100),
                                                   ch::milliseconds(21900), "AoE", false) },
                {}, true) },
        { "216338",
          Enemy("216338",
                vector<EnemyAbility>{ EnemyAbility(1241693, ch::milliseconds(6000),
                                                   ch::milliseconds(30300), "AoE", false) },
                {}, true) },
        { "223253",
          Enemy("223253",
                vector<EnemyAbility>{ EnemyAbility(448248, ch::milliseconds(4800),
                                                   ch::milliseconds(20600), "Volley", true) },
                {}, true) },
        { "216364",
          Enemy("216364",
                vector<EnemyAbility>{ EnemyAbility(433841, ch::milliseconds(5800),
                                                   ch::milliseconds(19000), "Volley", true) },
                {}, true) },
        // Priory of the Sacred
        // Flame-----------------------------------------------------------------------------------
        { "206696", Enemy("206696",
                          vector<EnemyAbility>{
                              EnemyAbility(427609, ch::milliseconds(20400), ch::milliseconds(23000),
                                           "Stop casting", false),
                              EnemyAbility(427621, ch::milliseconds(3800), ch::milliseconds(15700),
                                           "Impale bleed", false) },
                          {}, true) },
        { "221760",
          Enemy("221760",
                vector<EnemyAbility>{ EnemyAbility(444743, ch::milliseconds(9500),
                                                   ch::milliseconds(24300), "Volley", true) },
                {}, true) },
        { "212826",
          Enemy("212826",
                vector<EnemyAbility>{ EnemyAbility(448485, ch::milliseconds(5900),
                                                   ch::milliseconds(12100), "Tank Buster", false),
                                      EnemyAbility(448492, ch::milliseconds(14700),
                                                   ch::milliseconds(15700), "AoE", false) },
                {}, true) },
        { "212831",
          Enemy("212831",
                vector<EnemyAbility>{ EnemyAbility(427897, ch::milliseconds(10800),
                                                   ch::milliseconds(18200), "AoE", false) },
                {}, true) },
        { "239833",
          Enemy("239833",
                vector<EnemyAbility>{ EnemyAbility(424431, ch::milliseconds(26100),
                                                   ch::milliseconds(37600), "AoE", false) },
                {}, true) },
        { "206704",
          Enemy("206704",
                vector<EnemyAbility>{ EnemyAbility(448791, ch::milliseconds(15500),
                                                   ch::milliseconds(21700), "AoE", false) },
                {}, true) },
        { "206699",
          Enemy("206699",
                vector<EnemyAbility>{ EnemyAbility(446776, ch::milliseconds(7000),
                                                   ch::milliseconds(15800), "Leap bleed", false) },
                {}, true) },
};

const map<int, string> identifying_spells = {
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

const unordered_set<string> ignorable_events = {
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

const unordered_set<int> interrupt_ids = {
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

const map<string, int> interrupts_ids = {
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

const map<int, ch::seconds> interrupt_cooldown_m = {
        { 47528, ch::seconds(15) },   // Death Knight (Mind Freeze)
        { 183752, ch::seconds(15) },  // Demon Hunter (Disrupt)
        { 78675, ch::seconds(60) },   // Druid (Moonkin)
        { 106839, ch::seconds(15) },  // Druid (Skull Bash)
        { 351338, ch::seconds(20) },  // Evoker (Quell)
        { 187707, ch::seconds(15) },  // Hunter (Muzzle)
        { 2139, ch::seconds(24) },    // Mage (Counterspell)
        { 116705, ch::seconds(15) },  // Monk (Spear Hand Strike)
        { 96231, ch::seconds(15) },   // Paladin (Rebuke)
        { 15487, ch::seconds(45) },   // Priest (Silence)
        { 1766, ch::seconds(15) },    // Rogue (Kick)
        { 57994, ch::seconds(12) },   // Shaman (Wind Shear)
        { 19647, ch::seconds(24) },   // Warlock (Spell Lock)
        { 6552, ch::seconds(15) },    // Warrior (Pummel)
};

const unordered_set<int> crowd_control_ids = {
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
        // Warrior
        5246,  // Intimidating Shout
        46968  // Shockwave
};

const map<string, vector<AbilityState>> crowd_control_m = {
        { "Death Knight",
          {
              AbilityState(207127, ch::seconds(60)),  // Blinding Sleet
              AbilityState(207167, ch::seconds(90)),  // Gorefiend's Grasp
          } },
        { "Demon Hunter",
          {
              AbilityState(179057, ch::seconds(60)),  // Chaos Nova
              AbilityState(202138, ch::seconds(90)),  // Sigil of Silence
              AbilityState(207684, ch::seconds(90)),  // Sigil of Misery
              AbilityState(204598, ch::seconds(90)),  // Sigil of Chains
          } },
        { "Druid",
          {
              AbilityState(102359, ch::seconds(30)),  // Mass Entanglement
              AbilityState(102793, ch::seconds(60)),  // Ursol's Vortex
              AbilityState(132469, ch::seconds(30)),  // Typhoon
          } },
        { "Evoker",
          {
              AbilityState(371900, ch::seconds(90)),   // Landslide
              AbilityState(358269, ch::seconds(120)),  // Deep Breath
              AbilityState(368725,
                           ch::seconds(90)),  // Tail Swipe (Dracthyr Racial)
          } },
        { "Hunter",
          {
              AbilityState(109248, ch::seconds(45)),  // Binding Shot
          } },
        { "Mage",
          {
              AbilityState(122, ch::seconds(30)),     // Frost Nova
              AbilityState(31661, ch::seconds(45)),   // Dragon's Breath
              AbilityState(113724, ch::seconds(45)),  // Ring of Frost
              AbilityState(157981, ch::seconds(30)),  // Blast Wave
          } },
        { "Monk",
          {
              AbilityState(119381, ch::seconds(60)),  // Leg Sweep
              AbilityState(116844, ch::seconds(60)),  // Ring of Peace
          } },
        { "Paladin",
          {
              AbilityState(105421, ch::seconds(90)),  // Blinding Light
          } },
        { "Priest",
          {
              AbilityState(8122, ch::seconds(60)),  // Psychic Scream
          } },
        { "Shaman",
          {
              AbilityState(192058, ch::seconds(60)),  // Capacitor Totem
              AbilityState(51485, ch::seconds(30)),   // Earthgrab Totem (Talent)
              AbilityState(51490, ch::seconds(45)),   // Thunderstorm
              AbilityState(197214,
                           ch::seconds(40)),  // Sundering (Enhancement Talent)
          } },
        { "Warlock",
          {
              AbilityState(5484, ch::seconds(45)),   // Howl of Terror
              AbilityState(30283, ch::seconds(60)),  // Shadowfury
          } },
        { "Warrior",
          {
              AbilityState(5246, ch::seconds(90)),  // Intimidating Shout
              AbilityState(46968, ch::seconds(40))  // Shockwave (Talent)
          } }
};

const map<int, ch::seconds> crowd_control_cooldown_m = {
        // Death Knight
        { 207127, ch::seconds(60) },  // Blinding Sleet
        { 207167, ch::seconds(90) },  // Gorefiend's Grasp
        // Demon Hunter
        { 179057, ch::seconds(60) },  // Chaos Nova
        { 202138, ch::seconds(90) },  // Sigil of Silence
        { 207684, ch::seconds(90) },  // Sigil of Misery
        { 204598, ch::seconds(90) },  // Sigil of Chains
        // Druid
        { 102359, ch::seconds(30) },  // Mass Entanglement
        { 102793, ch::seconds(60) },  // Ursol's Vortex
        { 132469, ch::seconds(30) },  // Typhoon
        // Evoker
        { 371900, ch::seconds(90) },   // Landslide
        { 358269, ch::seconds(120) },  // Deep Breath
        { 368725, ch::seconds(90) },   // Tail Swipe (Dracthyr Racial)
        // Hunter
        { 109248, ch::seconds(45) },  // Binding Shot
        // Mage
        { 122, ch::seconds(30) },     // Frost Nova
        { 31661, ch::seconds(45) },   // Dragon's Breath
        { 113724, ch::seconds(45) },  // Ring of Frost
        { 157981, ch::seconds(30) },  // Blast Wave
        // Monk
        { 119381, ch::seconds(60) },  // Leg Sweep
        { 116844, ch::seconds(60) },  // Ring of Peace
        // Paladin
        { 105421, ch::seconds(90) },  // Blinding Light
        // Priest
        { 8122, ch::seconds(60) },  // Psychic Scream
        // Shaman
        { 192058, ch::seconds(60) },  // Capacitor Totem
        { 51485, ch::seconds(30) },   // Earthgrab Totem (Talent)
        { 51490, ch::seconds(45) },   // Thunderstorm
        { 197214, ch::seconds(40) },  // Sundering (Enhancement Talent)
        // Warlock
        { 5484, ch::seconds(45) },   // Howl of Terror
        { 30283, ch::seconds(60) },  // Shadowfury
        // Warrior
        { 5246, ch::seconds(90) },  // Intimidating Shout
        { 46968, ch::seconds(40) }  // Shockwave
};
