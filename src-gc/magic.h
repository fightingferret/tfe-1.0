/*
 *   Spells takes the arguments char_data, char_data, object, level  
 *
 *    Casting: caster, subject, object, level, -1
 *    Zapping: zapper, subject, object, level, duration
 *   Throwing: caster, subject, object, level, -2
 *   Quaffing: NULL, drinker, NULL, level, duration
 *    Dipping: dipper, NULL, object, level, -3
 *   Reacting: NULL, filler, object, level, -4
 *
 */


/*
 *   CASTING
 */


class Cast_Data
{
 public:
  cast_data*      next;
  int             mana;
  int            spell;
  int            times;
  int             wait;
  thing_data*   target;
  room_data*      room;
  bool         prepare;
  obj_data*    reagent  [ MAX_SPELL_WAIT ];
  
  Cast_Data( );
  ~Cast_Data( );
};


/*
 *   LEECH ROUTINES
 */


void   sprintf_leech    ( char*, int );
int    leech_mana       ( int, int, int = 0 );
int    leech_regen      ( int, int, int = 0 );
int    duration         ( int, int, int = 0 );
void   remove_leech     ( affect_data* );
void   remove_leech     ( char_data* );
int    leech_regen      ( char_data* );
int    leech_max        ( char_data* );
int    prep_max         ( char_data* );


/*
 *   ???
 */


int          spell_damage     ( int, int, int = 0 );
void         disrupt_spell    ( char_data*, bool = FALSE );
cast_data*   has_prepared     ( char_data*, int );
bool         makes_save       ( char_data*, char_data*, int, int, int );
bool         get_target       ( char_data*, cast_data*, char* );
bool         has_reagents     ( char_data*, cast_data* );
bool         can_replicate    ( char_data*, obj_data* obj );
void         spell_affect     ( char_data*, char_data*, int, int, int,
                                int, int = 0 );


/*
 *   SPELL TYPES
 */


#define STYPE_OFFENSIVE             0
#define STYPE_PEACEFUL              1
#define STYPE_SELF_ONLY             2
#define STYPE_OBJECT                3
#define STYPE_ROOM                  4
#define STYPE_WORLD                 5
#define STYPE_PEACEFUL_OTHER        6
#define STYPE_WEAPON                7
#define STYPE_DRINK_CON             8
#define STYPE_MOB_ONLY              9
#define STYPE_ANNOYING             10
#define STYPE_CORPSE               11
#define STYPE_RECALL               12
#define STYPE_WEAPON_ARMOR         13
#define STYPE_REPLICATE            14
#define STYPE_ASTRAL               15
#define MAX_STYPE                  16


extern const char* stype_name [ MAX_STYPE ];


/* 
 *   SPELL FUNCTIONS
 */


void   spell_update      ( char_data* );
void   heal_victim       ( char_data*, char_data*, int );


spell_func spell_acid_blast;
spell_func spell_acid_storm;
spell_func spell_animate_clay;
spell_func spell_animate_dead; 
spell_func spell_amnesia;
spell_func spell_armor;
spell_func spell_astral_gate;
spell_func spell_augury;
spell_func spell_balm;
spell_func spell_banishment;
spell_func spell_barkskin;
spell_func spell_bless;
spell_func spell_blind;
spell_func spell_blinding_light;
spell_func spell_blink;
spell_func spell_burning_hands;
spell_func spell_call_lightning;
spell_func spell_calm;
spell_func spell_chain_lightning;
spell_func spell_chilling_touch;
spell_func spell_confuse;
spell_func spell_continual_light;
spell_func spell_create_food;
spell_func spell_create_light;
spell_func spell_create_water;
spell_func spell_cure_light;
spell_func spell_cure_blindness;
spell_func spell_cure_critical;
spell_func spell_cure_disease;
spell_func spell_cure_poison;
spell_func spell_cure_serious;
spell_func spell_detect_invisible; 
spell_func spell_detect_magic;
spell_func spell_displace; 
spell_func spell_drain_life;
spell_func spell_eagle_eye;
spell_func spell_fear;
spell_func spell_findpath;
spell_func spell_find_familiar;
spell_func spell_find_mount;
spell_func spell_fireball;
spell_func spell_fire_shield;
spell_func spell_flame_strike;
spell_func spell_float;
spell_func spell_fly;
spell_func spell_freeze;
spell_func spell_gift_of_tongues;
spell_func spell_greater_animation;
spell_func spell_hallucinate;
spell_func spell_haste;
spell_func spell_heal;
spell_func spell_holy_wrath;
spell_func spell_ice_storm;
spell_func spell_identify;
spell_func spell_ignite_weapon;
spell_func spell_invulnerability;
spell_func spell_know_alignment;
spell_func spell_lesser_summoning;
spell_func spell_lightning_bolt;
spell_func spell_magic_mapping;
spell_func spell_magic_missile;
spell_func spell_maelstrom;
spell_func spell_minor_enchantment;
spell_func spell_major_enchantment;
spell_func spell_mists_sleep;
spell_func spell_mind_blade;
spell_func spell_neutralize;
spell_func spell_ogre_strength;
spell_func spell_pass_door;
spell_func spell_polymorph;
spell_func spell_prismic_missile;
spell_func spell_protect_evil;
spell_func spell_protect_good;
spell_func spell_protect_life;
spell_func spell_recall;
spell_func spell_remove_curse;
spell_func spell_replicate;
spell_func spell_request_ally;
spell_func spell_resist_acid;
spell_func spell_resist_cold;
spell_func spell_resist_fire;
spell_func spell_resist_shock;
spell_func spell_resurrect;
spell_func spell_revitalize;
spell_func spell_sanctify;
spell_func spell_sense_danger;
spell_func spell_sense_life;
spell_func spell_shock;
spell_func spell_silence;
spell_func spell_slay; 
spell_func spell_sleep;
spell_func spell_slow; 
spell_func spell_summon;
spell_func spell_surcease;
spell_func spell_tame;
spell_func spell_restoration;
spell_func spell_thorn_shield;
spell_func spell_transfer;
spell_func spell_true_sight;
spell_func spell_turn_undead;
spell_func spell_vitality;
spell_func spell_water_breathing;
spell_func spell_web;
spell_func spell_wizard_lock;
spell_func spell_youth;

spell_func spell_group_serious;
spell_func spell_cause_light; 
spell_func spell_cause_serious; 
spell_func spell_cause_critical; 
spell_func spell_harm; 
spell_func spell_faerie_fire; 
spell_func spell_invisibility; 
spell_func spell_protection_plants; 
spell_func spell_curse; 
spell_func spell_locust_swarm; 
spell_func spell_poison_cloud; 
spell_func spell_detect_hidden; 
spell_func spell_detect_good; 
spell_func spell_detect_evil; 
spell_func spell_infravision; 
spell_func spell_mystic_shield; 

spell_func spell_ion_shield;
spell_func spell_meteor_swarm;
spell_func spell_ice_lance;
spell_func spell_purify;
spell_func spell_wither;
spell_func spell_construct_golem;
spell_func spell_conflagration;
spell_func spell_conjure_elemental;
spell_func spell_poultice;
spell_func spell_scry;
spell_func spell_create_feast;
spell_func spell_hawks_view;
spell_func spell_group_critical;
spell_func spell_obscure;

/*
 *   NULL POINTER ROUTINES
 */


inline bool null_caster( char_data* ch, int spell )
{
  if( ch == NULL ) {
    bug( "%s: Null pointer for caster.", skill_table[spell].name );
    return TRUE;
    }

  return FALSE;
}


inline bool null_corpse( obj_data* corpse, int spell )
{
  if( corpse == NULL ) {
    bug( "%s: Null pointer for corpse.", skill_table[spell].name );
    return TRUE;
    }

  return FALSE;
}


