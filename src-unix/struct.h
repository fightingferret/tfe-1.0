/*
 *   DEFINED TYPES
 */


typedef class   Account_Data       account_data;
typedef class   Action_Data        action_data;
typedef class   Affect_Data        affect_data;
typedef class   Alias_Data         alias_data;
typedef class   Area_Data          area_data;
typedef class   Auction_Data       auction_data;
typedef class   Cast_Data          cast_data;
typedef class   Char_Data          char_data;
typedef class   Clan_Data          clan_data;
typedef class   Command_Data       command_data;
typedef class   Command_Queue      command_queue;
typedef class   Const_Data         const_data;
typedef class   Content_Array      content_array;
typedef class   Custom_Data        custom_data;
typedef class   Descr_Data         descr_data;
typedef class   Dice_Data          dice_data;
typedef class   Enemy_Data         enemy_data;
typedef class   Exit_Data          exit_data;
typedef struct  Extra_Data         extra_data;
typedef class   Clan_Data          clan_data;
typedef class   Help_Data          help_data;
typedef class   Index_Data         index_data;
typedef class   Info_Data          info_data;
typedef class   Link_Data          link_data;
typedef class   List_Data          list_data;
typedef class   Mob_Data           mob_data;
typedef class   Mprog_Data         mprog_data;
typedef class   Note_Data          note_data;
typedef class   Obj_Data           obj_data;
typedef class   Obj_Clss_Data      obj_clss_data;
typedef class   Oprog_Data         oprog_data;
typedef class   Path_Data          path_data;
typedef class   Pc_Data            pc_data;
typedef class   Pfile_Data         pfile_data;
typedef class   Player_Data        player_data;
typedef class   Program_Data       program_data;
typedef class   Quest_Data         quest_data;
typedef class   Recipe_Data        recipe_data;
typedef class   Recognize_Data     recognize_data;
typedef class   Reput_Data         reput_data;
typedef class   Request_Data       request_data;
typedef class   Reset_Data         reset_data;
typedef class   Room_Data          room_data;
typedef class   Rtable_Data        rtable_data;
typedef class   Share_Data         share_data;
typedef class   Shop_Data          shop_data;
typedef class   Skill_Type         skill_type;
typedef class   Species_Data       species_data;
typedef class   String             string;
typedef class   String_Fifo        string_fifo;
typedef class   Tell_Data          tell_data;
typedef class   Text_Data          text_data;
typedef class   Time_Data          time_data;
typedef class   Term_Type          term_type;
typedef class   Terrain_Type       terrain_type;
typedef class   Thing_Array        thing_array;
typedef class   Thing_Data         thing_data;
typedef class   Title_Array        title_array;
typedef class   Title_Data         title_data;
typedef class   Track_Data         track_data;
typedef class   Trainer_Data       trainer_data;
typedef class   Var_Data           var_data;
typedef class   Visible_Data       visible_data;
typedef class   Weather_Data       weather_data;
typedef class   Wizard_Data        wizard_data;

typedef class   Arg_Type           arg_type;
typedef class   Carg_Data          carg_data;
typedef class   Loop_Type          queue_type;
typedef class   Op_Type            op_type;
typedef class   Queue_Data         queue_data;
typedef class   Stack_Data         stack_data;

typedef class   Event_Data         event_data;


/*
 *   FUNCTION TYPE
 */


#define td typedef
#define cd char_data

td void         do_func     ( cd*, char* argument );
td bool         spell_func  ( cd*, char_data*, void*, int, int );
td void         path_func   ( cd*, cd*, char*, int = 0, int = 0 );
td bool         set_func    ( cd*, char*, const char*& );
td thing_data*  thing_func  ( thing_data*, cd*, thing_data* = NULL );
td void         event_func  ( event_data* );

#undef cd
#undef td


/*
 *   BASIC HEADERS
 */


#include "ctype.h"
#include "math.h"
#include "complex.h"
#include "string.h"

#include "machine.h"
#include "macros.h"

#include "bit.h"
#include "general.h"
#include "memory.h"
#include "string2.h"
#include "weather.h"


/* 
 *   CLASS TEMPLATES
 */


#include "array.h"


typedef class Array<Affect_Data*>   affect_array;
typedef class Array<Alias_Data*>    alias_array;
typedef class Array<Auction_Data*>  auction_array;
typedef class Array<Char_Data*>     char_array;
typedef class Array<Event_Data*>    event_array;
typedef class Array<Exit_Data*>     exit_array;
typedef class Array<Mob_Data*>      mob_array;
typedef class Array<Obj_Data*>      obj_array;
typedef class Array<Pfile_Data*>    pfile_array;
typedef class Array<Player_Data*>   player_array;
typedef class Array<Request_Data*>  request_array; 
typedef class Array<Room_Data*>     room_array;
typedef class Array<Extra_Data*>    extra_array;
typedef class Array<Visible_Data*>  visible_array;


/*
 *   CLASS HEADERS
 */


#include "affect.h"
#include "thing.h"
#include "object.h"


/*
 *   REMAINING HEADERS 
 */


#include "file.h"
#include "flag.h"

#include "abil.h"
#include "number.h"
#include "table.h"

#include "event.h"
#include "interp.h"

#include "char.h"
#include "pos.h"
#include "wear.h"

#include "network.h"
#include "option.h"

#include "fight.h"
#include "imm.h"
#include "player.h"

#include "account.h"
#include "auction.h"
#include "cards.h"
#include "channel.h"
#include "clan.h"
#include "code.h"
#include "group.h"
#include "library.h"
#include "liquid.h"
#include "list.h"
#include "look.h"
#include "magic.h"
#include "mob.h"
#include "move.h"
#include "note.h"
#include "program.h"
#include "recipe.h"
#include "reset.h"
#include "element.h"
#include "reput.h"
#include "room.h"
#include "set.h"
#include "shop.h"
#include "skill.h"
#include "thief.h"
#include "update.h"
#include "weight.h"

#include "output.h"
#include "color.h"


/*
 *   GENERAL CLASSES/STRUCTURES
 */


class Carg_Data
{
 public:
  char_data     *ch;
  char_data     *mob;
  obj_data      *obj1;
  obj_data      *obj2;
  room_data     *room;
};


class Quest_Data
{
 public:
  int              vnum;
  const char*   message;
  int            points;
};    


struct str_app_type
{
  int    tohit;
  int    todam;
  int    carry;
  int    wield;
};


struct    int_app_type
{
    int    learn;
};


struct    dex_app_type
{
  int    defensive;
};


#define OBJ_PANTS               2020
#define OBJ_SHIRT               2000
#define OBJ_SUCCOUR              286
#define OBJ_RATIONS                4
#define OBJ_CUP                    5 
#define OBJ_TORCH                 26
#define OBJ_SPELLBOOK              7
#define OBJ_SCROLL              1025
#define OBJ_FAQ                   57

#define OBJ_SHORT               3000 


/*
 *   GLOBAL CONSTANTS
 */


extern  const   char*   act_name [];
extern  const   char*   action_value [];
extern  const   char*   action_trigger [];
extern  const   char*   affect_location [];
extern  const   char   *anti_flags [];
extern  const   char   *burden_name [];
extern  const   char   *coin_name [];
extern  const   int    coin_value [];
extern  const   int    coin_vnum  [];
extern  const   char   *cont_flag_name [];
extern  const   char   *dflag_name [];
extern  const   char   *iflag_name [];
extern  const   char   *item_type_name [];
extern  const   char   *item_values [];
extern  const   char   *material_name [ MAX_MATERIAL ];
extern  const   char   *oflag_ident [];
extern  const   char   *oflag_name [];
extern  const   char   *mprog_trigger [];
extern  const   char   *month_name [];
extern  const   char   *mprog_value [ MAX_MPROG_TRIGGER ];
extern  const   char   *oprog_trigger [ MAX_OPROG_TRIGGER ];
extern  const   char   *plr_name [];
extern  const   char   *position_name [];
extern  const   char   *quest_name [];
extern  const   char   *rflag_name [];
extern  const   char   *restriction_flags [];
extern  const   char   *size_name [];
extern  const   char   *sex_name [];
extern  const   char   *size_flags []; 
extern  const   char   *trap_flags [];
extern  const   char   *weapon_attack [ MAX_WEAPON ];
extern  const   char   *where_name []; 

extern    const    struct    str_app_type    str_app        [31];
extern    const    struct    dex_app_type    dex_app        [31];

extern  quest_data       *quest_list   [ MAX_QUEST ];

extern char* help_greeting;
extern char* help_summary;
extern char* help_blank;

extern int max_pfile;

extern pfile_data** pfile_list;
extern int        ident_lookup  [ MAX_PFILE ];

extern int timeDrain;
extern int player_number;
extern int num_rooms;

/*
 *   GLOBAL FUNCTIONS
 */


do_func do_abilities;
do_func do_accounts;
do_func do_acode;
do_func do_adata;
do_func do_advance;
do_func do_aedit;
do_func do_affects;
do_func do_aflag;
do_func do_alias;
do_func do_allegiance;
do_func do_appearance;
do_func do_appraise;
do_func do_approve;
do_func do_areas;
do_func do_aset;
do_func do_ask;
do_func do_assist;
do_func do_astat;
do_func do_atalk;
do_func do_auction;
do_func do_avatar;
do_func do_backstab;
do_func do_balance;
do_func do_bamfin;
do_func do_bamfout;
do_func do_ban;
do_func do_bandage;
do_func do_bash;
do_func do_befriend;
do_func do_beep;
do_func do_define;
do_func do_berserk;
do_func do_bid;
do_func do_bite;
do_func do_brandish;
do_func do_build;
do_func do_buildchan;
do_func do_bugs;
do_func do_bury;
do_func do_buy;
do_func do_calculate;
do_func do_camouflage;
do_func do_cast;
do_func do_cedit;
do_func do_cflag;
do_func do_changes; 
do_func do_chant;
do_func do_chat;
do_func do_charge;
do_func do_clans;
do_func do_climb;
do_func do_close;
do_func do_color;
do_func do_commands;
do_func do_compare;
do_func do_configure;
do_func do_consent;
do_func do_consider;
do_func do_cook;
do_func do_ctell;
do_func do_custom;
do_func do_descript;
do_func do_dedit;
do_func do_delete;
do_func do_deny;
do_func do_deposit;
do_func do_dflag;
do_func do_dig;
do_func do_disguise;
do_func do_dip;
do_func do_disarm;
do_func do_disconnect;
do_func do_dismount;
do_func do_down;
do_func do_drink;
do_func do_drop;
do_func do_dset;
do_func do_dstat;
do_func do_east;
do_func do_eat;
do_func do_echo;
do_func do_emote;
do_func do_empty;
do_func do_enter;
do_func do_equipment;
do_func do_exits;
do_func do_exp;
do_func do_extract;
do_func do_gouge;
do_func do_fill;
do_func do_filter;
do_func do_flee;
do_func do_focus;
do_func do_follow;
do_func do_force;
do_func do_freeze;
do_func do_functions;
do_func do_garrote;
do_func do_get;
do_func do_give;
do_func do_glance;
do_func do_god;
do_func do_gossip;
do_func do_goto;
do_func do_guard;
do_func do_group;
do_func do_gtell;
do_func do_hands;
do_func do_hbug;
do_func do_hdesc;
do_func do_hedit;
do_func do_help;
do_func do_heist;
do_func do_hide;
do_func do_high;
do_func do_holylight;
do_func do_homepage;
do_func do_hset;
do_func do_identity;
do_func do_identify;
do_func do_iflag;
do_func do_ignite;
do_func do_immtalk;
do_func do_imprison;
do_func do_index;
do_func do_info;
do_func do_inspect;
do_func do_introduce;
do_func do_inventory;
do_func do_invis;
do_func do_junk;
do_func do_keywords;
do_func do_kick;
do_func do_kill;
do_func do_knock;
do_func do_label;
do_func do_lag;
do_func do_language;
do_func do_last;
do_func do_ledit;
do_func do_leech;
do_func do_level;
do_func do_lie;
do_func do_list;
do_func do_load;
do_func do_lock;
do_func do_log;
do_func do_look;
do_func do_lset;
do_func do_lstat;
do_func do_mail;
do_func do_map;
do_func do_marmor;
do_func do_mdesc;
do_func do_medit;
do_func do_meditate;
do_func do_melee;
do_func do_melt;
do_func do_memory;
do_func do_message;
do_func do_mfind;
do_func do_mflag;
do_func do_mload;
do_func do_mlog;
do_func do_motd;
do_func do_mount;
do_func do_move;
do_func do_mpcode;
do_func do_mpdata;
do_func do_mpedit;
do_func do_mpflag;
do_func do_mpset;
do_func do_mpstat;
do_func do_mreset;
do_func do_mset;
do_func do_mstat;
do_func do_mwhere;
do_func do_name;
do_func do_noemote;
do_func do_north;
do_func do_noshout;
do_func do_notell;
do_func do_notes;
do_func do_odesc;
do_func do_oedit;
do_func do_oextra;
do_func do_ofind;
do_func do_oflag;
do_func do_oload;
do_func do_olog;
do_func do_ooc;
do_func do_opcode;
do_func do_opdata;
do_func do_opedit;
do_func do_opset;
do_func do_opstat;
do_func do_open;
do_func do_options;
do_func do_order;
do_func do_oset;
do_func do_ostat;
do_func do_owhere;
do_func do_pardon;
do_func do_password;
do_func do_pbug;
do_func do_peace;
do_func do_peek;
do_func do_pets;
do_func do_pick;
do_func do_polymorph;
do_func do_practice;
do_func do_pray;
do_func do_probe;
do_func do_prompt;
do_func do_prepare;
do_func do_ps;
do_func do_pull;
do_func do_purchase;
do_func do_push;
do_func do_punch;
do_func do_purge;
do_func do_put;
do_func do_qedit;
do_func do_qlook;
do_func do_qremove;
do_func do_qset;
do_func do_qstat;
do_func do_quaff;
do_func do_quests;
do_func do_quit;
do_func do_qwhere;
do_func do_qwho;
do_func do_rbug;
do_func do_read;
do_func do_reboot;
do_func do_recho;
do_func do_recite;
do_func do_rdesc;
do_func do_redit;
do_func do_reimburse;
do_func do_relations;
do_func do_rename;
do_func do_repair;
do_func do_reputation;
do_func do_remove;
do_func do_rent;
do_func do_reply;
do_func do_request;
do_func do_rescue;
do_func do_reset;
do_func do_rest;
do_func do_restore;
do_func do_return;
do_func do_review;
do_func do_rflag;
do_func do_rlog;
do_func do_rmwhere;
do_func do_rname;
do_func do_roomlist;
do_func do_rowhere;
do_func do_rset;
do_func do_rstat;
do_func do_rtable;
do_func do_rtwhere;
do_func do_sacrifice;
do_func do_save;
do_func do_say;
do_func do_scan;
do_func do_score;
do_func do_search;
do_func do_sell;
do_func do_shcustom;
do_func do_shedit;
do_func do_shflag;
do_func do_shoot;
do_func do_shout;
do_func do_shutdown;
do_func do_skills;
do_func do_skin;
do_func do_sit;
do_func do_slay;
do_func do_sleep;
do_func do_slookup;
do_func do_smoke;
do_func do_sneak;
do_func do_snoop;
do_func do_socials;
do_func do_source;
do_func do_south;
do_func do_spells;
do_func do_spin_kick;
do_func do_split;
do_func do_sset;
do_func do_stand;
do_func do_statistics;
do_func do_steal;
do_func do_sweep;
do_func do_switch;
do_func do_system;
do_func do_tedit;
do_func do_tell;
do_func do_tfind;
do_func do_throw;
do_func do_time;
do_func do_title;
do_func do_to;
do_func do_train;
do_func do_transfer;
do_func do_energize;
do_func do_track;
do_func do_trust;
do_func do_tset;
do_func do_tstat;
do_func do_typo;
do_func do_unlock;
do_func do_untrap;
do_func do_up;
do_func do_users;
do_func do_value;
do_func do_vote; 
do_func do_wake;
do_func do_wanted;
do_func do_wear;
do_func do_weather;
do_func do_west;
do_func do_where;
do_func do_whistle;
do_func do_whisper;
do_func do_who;
do_func do_whois;
do_func do_wimpy;
do_func do_withdraw;
do_func do_wizlock;
do_func do_write;
do_func do_yell;
do_func do_zap;


#define CD    char_data
#define CSTD  CAST_DATA
#define EXD   EXIT_DATA
#define EXT   extra_data
#define LD    list_data
#define MID   species_data
#define OD    obj_data
#define OID   obj_clss_data
#define PD    PROGRAM_DATA
#define RD    room_data
#define CNTC  const char


//long    random       ( void );
void    srandom      ( int seed );

void    boot_db        ( void );
void    shop_update     ( void );
int    interpolate    ( int level, int value_00, int value_32 );
bool    str_cmp        ( const char *astr, const char *bstr );
bool    str_prefix    ( const char *astr, const char *bstr );
bool    join_fight      ( char_data *victim, char_data *ch,
                          char_data *rch );
int    get_obj_number    ( obj_data *obj );
quest_data * get_quest_index ( int vnum );


bool   are_allied         ( char_data*, char_data* );
bool   can_drop_obj       ( char_data *ch, obj_data *obj );
bool   does_hit           ( char_data*, char_data*, char*, obj_data* );
bool   is_aggressive      ( char_data*, char_data* );
bool   is_confused_pet    ( char_data* );
bool   is_humanoid        ( char_data* );
bool   is_familiar        ( char_data* );
bool   is_mob             ( char_data* );
bool   is_number          ( char *arg );
bool   isperiod           ( char letter );
bool   is_same_group      ( char_data*, char_data* ); 
bool   is_switched        ( char_data* );
bool   is_wanted          ( char_data *ch, int race );
bool   load_char          ( link_data*, char*, const char* );
bool   not_player         ( char_data* );
bool   pet_help           ( char_data* );
bool   player_in_room     ( room_data* );
bool   remove_coins       ( char_data*, int, char* ); 
bool   room_is_dark       ( room_data *room );

char   *coin_phrase       ( char_data* );
char   *coin_phrase       ( int* );
char   *reputation_name   ( int );
char   *coin_phrase       ( int* );
char   *edit_string       ( char_data*, char*, char*, int );
char*  one_condition      ( char *argument, char *cond );
char*  one_line           ( char *argument, char *line );

const char *room_name     ( room_data* );
const char *number_pos_word  ( int ); 

CD*    find_keeper        ( char_data* );
CD     *rand_victim       ( char_data* );
CD     *rand_player       ( room_data* );
CD     *random_pers       ( room_data* );
CD     *get_char_group    ( char_data*, char* );


CNTC*  number_suffix      ( int );
CNTC*  damage_color       ( char_data*,  char_data*, char_data* );
CNTC   *condition_word    ( char_data *ch );

int    dice_string        ( char *string );
int    get_damage         ( char_data*, obj_data*, char );
int    get_money          ( char_data *ch );
int    get_random_door    ( room_data* );
int    get_reputation     ( char_data *ch, int race );
int    leech_regen        ( char_data* );
int    number_percent     ( void );
int    number_door        ( void );
int    number_bits        ( int );
int    mana_absorption    ( char_data* );
int    skill_index        ( const char* );
int    smash_argument     ( char*, char* );
int    pack_int           ( int* );
int    xp_compute         ( char_data* );

int    *unpack_int        ( int ); 

RD*    find_location      ( char_data*, char* );
RD*    get_temple         ( char_data* );

void   advance_level      ( char_data*, bool );
void   area_update        ( void );
void   att_message        ( char_data*, char_data*, int, char* );
void   calc_resist        ( char_data *ch );
void   check_string       ( char_data *ch, char *string );
void   check_killer       ( char_data *ch, char_data *victim );
void   check_mount        ( char_data* );
void   clear_queue        ( char_data* );
void   clear_queue        ( obj_data* );
void   clear_pager        ( char_data *ch );
void   death              ( char_data*, char_data*, char* );
void   death              ( char_data*, char_data*, char* );
void   enchant_object     ( obj_data* ); 
void   enter_game         ( char_data* );
void   format             ( char*, const char* );
void   format_tell        ( char *input, char *output );
void   free_prog          ( program_data*& );
void   gain_drunk         ( char_data*, int );
void   get_obj            ( char_data*, obj_data*, obj_data* );
void   group_gain         ( char_data*, char_data* );
void   help_link          ( link_data*, const char* );
void   info_race          ( int fromRace, int toRace, int flag );
void   init_memory        ( void );
void   interpret          ( char_data *ch, char *argument );
void   load               ( FILE*, reset_data*& );
void   load_dictionary    ( void );
void   load_tables        ( void );
void   lose_level         ( char_data *ch );
void   make_tracks        ( char_data*, room_data*, int );
void   modify_affect      ( char_data *ch, affect_data *affect, bool fAdd );
void   modify_pfile       ( char_data *ch );
void   modify_reputation  ( char_data*, char_data*, int );
void   multi_hit          ( char_data *ch, char_data *victim );
void   oset_obj           ( obj_data*, char* );
void   process_tell       ( char_data*, char_data*, char* );
void   remove_pets        ( char_data *victim );
void   save_factions      ( void );
void   save_mail          ( pfile_data* );
void   save_mobs          ( void );
void   save_objects       ( void );
void   save_quests        ( void );
void   save_tables        ( void );
void   send_to_area       ( const char*, area_data* );
void   set_alloy          ( obj_data*, int );
void   set_skills         ( void );
void   set_title          ( char_data*, char* );
void   show_map           ( char_data*, int, int );
void   sleep              ( char_data* );
void   start_spell        ( char_data*, cast_data* );
void   strcat_cond        ( char*, char_data*, char_data* ); 
void   summon_help        ( char_data*, char_data* );
void   unregister_reset   ( char_data* );
void   unseat             ( char_data* );
void   update_faction     ( char_data* );
void   update_handler     ( void );
void   update_links       ( void );
void   update_pos         ( char_data* );
void   update_quest       ( char_data *ch );
void   update_queue       ( void );
void   update_score       ( char_data* );
void   violence_update    ( void );
void   write              ( FILE*, action_data* );
void   write              ( FILE*, reset_data* );
void   write              ( player_data* );
void   wtb                ( link_data*, const char *txt );
void   zero_exp           ( species_data* );


#undef    CD
#undef    CSTD
#undef    CNTC
#undef    LD
#undef    MID
#undef    OD
#undef    OID
#undef    RD
#undef    SF
#undef    PD

