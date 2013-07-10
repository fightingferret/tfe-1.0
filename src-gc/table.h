/*
 *   LOCAL DEFINITIONS
 */


#define MAX_SOCIAL                500
#define MAX_SPELL_ACT             225
#define MAX_LIQUID                 50
#define MAX_TOWN                   10
#define MAX_METAL                  25
#define MAX_GROUP                 200
#define MAX_RACE                  180
#define MAX_AFF_CHAR              150
#define MAX_AFF_OBJ                50
#define MAX_COMMAND               400
#define MAX_CMD_CAT                30
#define MAX_CLSS                    9 
#define MAX_BUILD                  50
#define MAX_HELP_CAT               30
#define MAX_NATION                 15
#define MAX_ASTRAL                 25
#define MAX_RELIGION               20
#define MAX_ALIGNMENT               9

#define MAX_FIELD                  30

#define TABLE_SOC_DEFAULT           0
#define TABLE_SOC_HUMAN             1
#define TABLE_SOC_ELF               2
#define TABLE_SOC_GNOME             3
#define TABLE_SOC_DWARF             4
#define TABLE_SOC_HALFLING          5
#define TABLE_SOC_ENT               6
#define TABLE_SOC_CENTAUR           7
#define TABLE_SOC_LIZARD            8
#define TABLE_SOC_OGRE              9
#define TABLE_SOC_TROLL            10
#define TABLE_SOC_ORC              11
#define TABLE_SOC_GOBLIN           12
#define TABLE_SOC_VYAN             13
#define TABLE_SPELL_ACT            14
#define TABLE_LIQUID               15
#define TABLE_SPELL_DATA           16
#define TABLE_TOWN                 17
#define TABLE_SKILL                18
#define TABLE_METAL                19
#define TABLE_NATION               20
#define TABLE_GROUP                21
#define TABLE_RACE                 22
#define TABLE_PLYR_RACE            23
#define TABLE_AFF_CHAR             24
#define TABLE_AFF_OBJ              25
#define TABLE_COMMAND              26
#define TABLE_CMD_CAT              27
#define TABLE_CLSS                 28
#define TABLE_STARTING             29
#define TABLE_TEDIT                30
#define TABLE_BUILD                31
#define TABLE_HELP_CAT             32
#define TABLE_ASTRAL               33
#define TABLE_RELIGION             34
#define TABLE_ALIGNMENT            35
#define MAX_TABLE                  36


#define MAX_ENTRY_NATION        table_max[ TABLE_NATION ]
#define MAX_ENTRY_GROUP         table_max[ TABLE_GROUP ]
#define MAX_ENTRY_COMMAND       table_max[ TABLE_COMMAND ]
#define MAX_ENTRY_AFF_CHAR      table_max[ TABLE_AFF_CHAR ]
#define MAX_ENTRY_AFF_OBJ       table_max[ TABLE_AFF_OBJ ]
#define MAX_ENTRY_CMD_CAT       table_max[ TABLE_CMD_CAT ]
#define MAX_ENTRY_LIQUID        table_max[ TABLE_LIQUID ]
#define MAX_ENTRY_BUILD         table_max[ TABLE_BUILD ]
#define MAX_ENTRY_HELP_CAT      table_max[ TABLE_HELP_CAT ]
#define MAX_ENTRY_TOWN          table_max[ TABLE_TOWN ]
#define MAX_ENTRY_ASTRAL        table_max[ TABLE_ASTRAL ]
#define MAX_ENTRY_NATION        table_max[ TABLE_NATION ]
#define MAX_ENTRY_RACE          table_max[ TABLE_RACE ]
#define MAX_ENTRY_RELIGION      table_max[ TABLE_RELIGION ]
#define MAX_ENTRY_METAL         table_max[ TABLE_METAL ]
#define MAX_ENTRY_ALIGNMENT     table_max[ TABLE_ALIGNMENT ]

#define VAR_CHAR           0
#define VAR_INT            1
#define VAR_SA             2
#define CNST_CHAR          3
#define VAR_SKILL          4
#define VAR_SIZE           5
#define VAR_FUNC           6
#define VAR_CC             7
#define VAR_POS            8
#define VAR_AFF            9
#define VAR_OBJ           10
#define VAR_DICE          11
#define VAR_SCAT          12
#define VAR_STYPE         13
#define VAR_PERM          14
#define VAR_FORMULA       15
#define VAR_LEECH         16
#define VAR_BOOL          17
#define VAR_ALIGN         18
#define VAR_LANG          19
#define VAR_AFF_LOC       20
#define VAR_LOC           21
#define VAR_CENT          22
#define VAR_SEX           23 
#define VAR_TEMP          24
#define VAR_BLANK         25
#define VAR_DELETE        26


/*
 *   TYPE DEFINITIONS
 */


typedef class  Aff_Char_Type      aff_char_type;
typedef class  Aff_Obj_Type       aff_obj_type;
typedef class  Entry_Data         entry_data;
typedef class  Nation_Data        nation_data;
typedef class  Group_Data         group_data;
typedef class  Race_Data          race_data;
typedef class  Spell_Data         spell_data;
typedef class  Social_Type        social_type;
typedef class  Liquid_Type        liquid_type;
typedef class  Town_Type          town_type;
typedef class  Metal_Type         metal_type;
typedef class  Category_Data      category_data;
typedef class  Plyr_Race_Data     plyr_race_data;
typedef class  Clss_Type          clss_type;
typedef class  Command_Type       command_type;
typedef class  Starting_Data      starting_data;
typedef class  Tedit_Data         tedit_data;
typedef class  Spell_Act_Type     spell_act_type;
typedef class  Religion_Data      religion_data;
typedef class  Alignment_Data     alignment_data;


/*
 *   STRUCTURES 
 */


class Alignment_Data
{
 public:
  const char*     name;
  const char*   abbrev;
};


class Aff_Char_Type
{
 public:
  const char*       name;
  char*          id_line;
  char*       score_name;
  char*           msg_on;
  char*      msg_on_room;
  char*          msg_off;
  char*     msg_off_room;
  int           location;
  char*         modifier;
};


class Aff_Obj_Type
{
 public:
  const char*       name;
  char*           msg_on;
  char*          msg_off;
  int           location;
};


class Nation_Data
{
 public:
  const char*      name;
  char*          abbrev;
  int              room  [ 2 ];
  int            temple;
  int              race  [ MAX_PLYR_RACE ];
  int         alignment  [ MAX_ALIGNMENT ];
};


class Command_Type
{
 public:
  char*             name;
  char*             help;
  char*        func_name;
  do_func*      function;
  int           position;
  int              level  [ 2 ];
  int             reqlen;
  int            disrupt;
  int             reveal;
  int              queue;
  int           category;
  time_data     max_time;
  int              calls;
  time_data   total_time;

  Command_Type( ) {
    function    = NULL;
    max_time    = 0;
    calls       = 0;
    total_time  = 0;
    return;
    };
};


class Entry_Data
{
 public:
  void     *offset;
  int      type;
};


class Plyr_Race_Data
{
 public:
  const char*     name;
  const char*   plural;
  int             size;
  int           weight;
  int           resist  [ MAX_RESIST ];
  int           affect  [ AFFECT_INTS ];  
  int       stat_bonus  [ 5 ];
  int         hp_bonus;
  int       mana_bonus;
  int       move_bonus;
  int       start_room  [ 3 ];
  int           portal;
  int        start_age;
  int        life_span;
  int       alignments;
  int         language;
  bool            open;
};


class Clss_Type
{
 public:
  const char*        name;
  const char*      abbrev;    
  int             hit_min;    
  int             hit_max;    
  int            mana_min;
  int            mana_max;
  int            move_min;
  int            move_max;
  int           hit_bonus;
  int          mana_bonus;
  int          move_bonus;
  int              resist  [ MAX_RESIST ];
  int          alignments;
  bool               open;
};


class Group_Data
{
 public:
  const char*    name;
};


class Liquid_Type
{
 public:
  const char*     name;
  char*          color;
  int           hunger;
  int           thirst;
  int          alcohol;
  int             cost;
  bool          create;
  int            spell;
};


class Metal_Type
{
 public:
  char*           name;
  int             cost;
  int           weight;
  int             mana;
  int            armor;
  int          enchant;
  int        save_fire;
  int        save_cold;
  int        save_acid;
  char*       msg_fire;
  char*       msg_cold;
  char*       msg_acid;
  char*      rust_name;
  char*           rust  [3];   
};


class Race_Data
{
 public:
  const char*      name;
  char*          abbrev;
  char*           track;
}; 

  
class Skill_Type
{
 public:
  const char*       name;
  int          pre_skill  [ 2 ];
  int          pre_level  [ 2 ];
  int           category;
  int          prac_cost  [ MAX_CLSS ];
  int              level  [ MAX_CLSS ];
};


class Social_Type
{
 public:
  char*              name;
  int            position;
  int          aggressive;
  int             disrupt;
  int              reveal;
  char*       char_no_arg;
  char*     others_no_arg;
  char*        char_found;
  char*      others_found;
  char*        vict_found;
  char*        vict_sleep;
  char*         char_auto;
  char*       others_auto;
  char*          dir_self;
  char*        dir_others;
  char*          obj_self;
  char*        obj_others;
  char*       ch_obj_self;
  char*     ch_obj_others;
  char*     ch_obj_victim;
  char*      ch_obj_sleep;
  char*     self_obj_self;
  char*   self_obj_others;
};


class Spell_Act_Type
{
 public:
  char*            name;
  char*      self_other;
  char*    victim_other;
  char*    others_other;
  char*       self_self;
  char*     others_self;
};


class Spell_Data
{
 public:
  char*             name;
  int            prepare;
  int               wait;
  char*           damage;
  char*            regen;
  char*       leech_mana;
  char*        cast_mana;
  char*         duration;
  int               type;
  int           location;
  spell_func*   function;
  int             action  [ 5 ];
  int            reagent  [ 5 ];
};


class Tedit_Data
{
 public:
  char*    name;
  int      edit;
  int      new_delete;
};


class Town_Type
{ 
  public:
    char*     name;
    int     recall;
};


class Category_Data
{
  public:
    const char*    name;
    int           level;
};


class Starting_Data
{
  public:
    char*     name;
    int       skill  [ 5 ];
    int       level  [ 5 ];
    int      object  [ 10 ]; 
}; 


class Religion_Data
{
  public:
    char*          name;
    int             sex;
    int      alignments;
    int         classes;
};


/*
 *   DEFINITIONS
 */


#define AFF_NONE                   -1 
#define AFF_ARMOR                   0
#define AFF_BLESS                   1
#define AFF_BLIND                   2
#define AFF_DISPLACE                3
#define AFF_CHILL                   4
#define AFF_CURSE                   5
#define AFF_DETECT_EVIL             6
#define AFF_DETECT_HIDDEN           7
#define AFF_SEE_INVIS               8 
#define AFF_DETECT_MAGIC            9
#define AFF_FAERIE_FIRE            10
#define AFF_FIRE_SHIELD            11
#define AFF_HIDE                   12
#define AFF_INFRARED               13 
#define AFF_INVISIBLE              14
#define AFF_WRATH                  15
#define AFF_POISON                 16
#define AFF_PROTECT                17
#define AFF_SANCTUARY              18
#define AFF_SLEEP                  19
#define AFF_SNEAK                  20
#define AFF_REGENERATION           21
#define AFF_SPEED                  22
#define AFF_WATER_WALKING          23
#define AFF_WATER_BREATHING        24
#define AFF_INVULNERABILITY        25
#define AFF_ENTANGLED              26
#define AFF_CONFUSED               27
#define AFF_HALLUCINATE            28
#define AFF_SLOW                   29
#define AFF_PROT_PLANTS            30
#define AFF_VITALITY               31 
#define AFF_DETECT_GOOD            32
#define AFF_LIFE_SAVING            33
#define AFF_SLEEP_RESIST           34
#define AFF_RESIST_POISON          35
#define AFF_OGRE_STRENGTH          36
#define AFF_SILENCE                37
#define AFF_TONGUES                38
#define AFF_CONTINUAL_LIGHT        39
#define AFF_PLAGUE                 40
#define AFF_TOMB_ROT               41
#define AFF_RABIES                 42 
#define AFF_PARALYSIS              43
#define AFF_FLOAT                  44
#define AFF_BARKSKIN               45
#define AFF_PASS_DOOR              46
#define AFF_AXE_PROF               47
#define AFF_SWORD_PROF             48
#define AFF_BOW_PROF               49
#define AFF_LIGHT_SENSITIVE        50
#define AFF_DEATH                  51
#define AFF_SENSE_DANGER           52
#define AFF_RESIST_FIRE            53
#define AFF_RESIST_COLD            54
#define AFF_HASTE                  55
#define AFF_PROTECT_EVIL           56
#define AFF_PROTECT_GOOD           57
#define AFF_FLY                    58
#define AFF_SENSE_LIFE             59
#define AFF_TRUE_SIGHT             60
#define AFF_RESIST_ACID            61
#define AFF_RESIST_SHOCK           62
#define AFF_THORN_SHIELD           63
#define AFF_CHOKING                64
#define AFF_ION_SHIELD             65
#define AFF_CAMOUFLAGE             66
#define AFF_SEE_CAMOUFLAGE         67


#define AFF_BURNING                0
#define AFF_FLAMING                1
#define AFF_POISONED               2


#define CLSS_MAGE                  0
#define CLSS_CLERIC                1
#define CLSS_THIEF                 2
#define CLSS_WARRIOR               3
#define CLSS_PALADIN               4
#define CLSS_RANGER                5
#define CLSS_PSIONIC               6
#define CLSS_MONK                  7
#define CLSS_BARD                  8


#define GROUP_NONE                  0


#define LIQ_WATER                   0
#define LIQ_ACID                   22
#define LIQ_POISON                 23
#define LIQ_HOLY_WATER             24


#define NATION_NONE                 0
#define NATION_IMMORTAL             1  
#define NATION_SECOMBER             2


#define REL_NONE                    0


/*
 *   VARIABLES
 */

#define ec extern class

ec Social_Type      social_table      [ MAX_PLYR_RACE+1 ][ MAX_SOCIAL ];
ec Spell_Act_Type   spell_act_table   [ MAX_SPELL_ACT ];
ec Liquid_Type      liquid_table      [ MAX_LIQUID ];
ec Town_Type        town_table        [ MAX_TOWN ]; 
ec Spell_Data       spell_table       [ MAX_SPELL ];     
ec Skill_Type       skill_table       [ MAX_SKILL ];
ec Metal_Type       material_table    [ MAX_METAL ];
ec Nation_Data      nation_table      [ MAX_NATION ];
ec Group_Data       group_table       [ MAX_GROUP ];
ec Race_Data        race_table        [ MAX_RACE ];
ec Plyr_Race_Data   plyr_race_table   [ MAX_PLYR_RACE ];
ec Aff_Char_Type    aff_char_table    [ MAX_AFF_CHAR ];
ec Aff_Obj_Type     aff_obj_table     [ MAX_AFF_OBJ ];
ec Command_Type     command_table     [ MAX_COMMAND ];
ec Category_Data    cmd_cat_table     [ MAX_CMD_CAT ];
ec Clss_Type        clss_table        [ MAX_CLSS ];
ec Starting_Data    starting_table    [ MAX_CLSS+MAX_PLYR_RACE+1 ];
ec Tedit_Data       tedit_table       [ MAX_TABLE ];
ec Recipe_Data      build_table       [ MAX_BUILD ];
ec Category_Data    help_cat_table    [ MAX_HELP_CAT ];
ec Town_Type        astral_table      [ MAX_ASTRAL ]; 
ec religion_data    religion_table    [ MAX_RELIGION ]; 
ec alignment_data   alignment_table   [ MAX_ALIGNMENT ];

ec entry_data       table_entry       [ MAX_TABLE-MAX_PLYR_RACE ][ MAX_FIELD ];

#undef ec

extern const char**             table_field     [ MAX_TABLE ];
extern int                      table_max       [ MAX_TABLE ];


/*
 *   FUNCTIONS
 */


void  init_commands      ( void );
void  init_spells        ( void );
void  sort_socials       ( void );












