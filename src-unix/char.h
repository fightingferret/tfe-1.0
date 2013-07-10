extern player_array   player_list;
extern mob_array      mob_list;


/*
 *   CREATURE CLASS AND DERIVED SUB-CLASSES
 */


class Char_Data : public Thing_Data
{
 public: 
  char_data*     next_on_obj;
  char_data*       next_list;
  char_data*           mount;
  char_data*           rider;
  char_data*           reply;
  char_data*        fighting;
  descr_data*          descr;
  species_data*      species;

  event_data          active;
  command_queue    cmd_queue;

  char_data*          leader;
  char_array       followers;  
  char_array         seen_by;
  char_array        known_by;
  char_array      aggressive;
  exit_array      seen_exits;
  affect_array    leech_list;
  affect_array      affected;
  content_array      wearing;

  cast_data*            cast;
  reset_data*          reset;
  link_data*            link;
  pc_data*            pcdata;
  share_data*         shdata;
  enemy_data*          enemy;
  shop_data*           pShop;
  cast_data*         prepare;
  obj_data*          pos_obj;
  char*             pet_name;
  int                 played;
  int                  timer;

  room_data*     was_in_room;
  room_data*         in_room;

  time_t               logon;
  time_t           save_time; 

  int                    hit;
  int               base_hit;
  int                   mana;
  int              base_mana;
  int                   move;
  int              base_move;

  int                mod_hit;
  int               mod_move;
  int               mod_mana;
  int                max_hit;
  int               max_move;
  int               max_mana;

  int                mod_str;
  int                mod_int;
  int                mod_wis;
  int                mod_dex;
  int                mod_con;

  int              mod_resist  [ MAX_RESIST ];
  int               mod_armor;
  int             affected_by  [ AFFECT_INTS ];
  int                  status;
  int                     exp;
  int                 hitroll;
  int                 damroll;
  int              move_regen;
  int              mana_regen;
  int               hit_regen;
  int           room_position;
  int                     sex;

  Char_Data( );
  virtual ~Char_Data( );

  /* BASE */

  int   Type     ( )  { return CHAR_DATA; }
  void  Extract  ( );  

  /* NAME/KEYWORDS */

  const char*   Keywords        ( char_data* );
  const char*   Name            ( char_data* = NULL, int = 1, bool = FALSE );
  const char*   Seen_Name       ( char_data*, int = 1, bool = FALSE );
  const char*   Long_Name       ( char_data*, int = 1 );
  const char*   Show            ( char_data*, int = 1 );
  const char*   real_name       ( );  
  const char*   Location        ( content_array* = NULL );
  void          Look_At         ( char_data* );

  const char*   Him_Her         ( char_data* = NULL );
  const char*   His_Her         ( char_data* = NULL );
  const char*   He_She          ( char_data* = NULL );

  /* PROPERTIES */

  bool Can_See    ( bool = FALSE );
  bool Seen       ( char_data* );

  bool look_same  ( thing_data*, thing_data* );
  bool look_same  ( obj_data*, obj_data* );
  bool look_same  ( char_data*, char_data* );

  bool         Is_Valid        ( )  { return valid == CHAR_DATA; }
  bool         In_Game         ( );
  void         To              ( content_array* );
  void         To              ( thing_data* );
  thing_data*  From            ( int = 1 );

  virtual int  Size             ( )  = 0;
          int  Weight           ( int = -1 );
          int  Empty_Weight     ( int = -1 );
          int  Capacity         ( );
          int  Empty_Capacity   ( );

  /* AFFECTS */

  bool Sees_Invis    ( );
  bool Sees_Hidden   ( );
  bool detects_good  ( );
  bool detects_evil  ( );

  /* STATS */

  int            Strength        ( );
  int            Intelligence    ( );
  int            Wisdom          ( );
  int            Dexterity       ( );
  int            Constitution    ( );

  float          Hitroll         ( obj_data* );
  float          Damroll         ( obj_data* );
  int            Hit_Regen       ( );
  int            Move_Regen      ( );
  int            Mana_Regen      ( );

  virtual float  Mean_Hp         ( )  = 0;
  virtual float  Mean_Mana       ( )  = 0; 
  virtual float  Mean_Move       ( )  = 0;

  /* OTHER */

  bool           can_float              ( );
  bool           can_swim               ( );
  bool           can_breath_underwater  ( );
  bool           can_fly                ( );

  bool           Can_Move               ( exit_data* );
  bool           can_buy                ( obj_data*, char_data* );
  bool           can_edit               ( obj_clss_data* );
  bool           can_edit               ( species_data* );
  bool           check_skill            ( int );
  bool           save_vs_poison         ( int ); 

          bool           Recognizes      ( char_data* );
          bool           Befriended      ( char_data* ); 
          bool           Befriended      ( pfile_data* ); 
          bool           Ignoring        ( char_data* );
          bool           Filtering       ( char_data* );
  virtual bool           Accept_Msg      ( char_data* )  = 0;

  obj_data*      Wearing         ( int, int = -1 );

  int            Save_Magic      ( );
  int            Save_Fire       ( );
  int            Save_Cold       ( );
  int            Save_Mind       ( );
  int            Save_Shock      ( );
  int            Save_Acid       ( );
  int            Save_Poison     ( );

  int            can_carry_n     ( );
  int            can_carry_w     ( );
  int            get_burden      ( );
  int            get_skill       ( int );
  time_t         time_played     ( );

  void          improve_skill   ( int );
};


/*
 *   PLAYER DATA
 */


class Reput_Data
{
 public:
  int      nation  [ MAX_NATION ];  
  int   alignment  [ MAX_ALIGNMENT ]; 
  int       blood;
  int        gold; 
  int       magic; 

  Reput_Data( ) {
    blood = 0;
    gold  = 0;
    magic = 0;

    vzero( nation,    MAX_NATION );
    vzero( alignment, MAX_ALIGNMENT ); 
    }
};

 
class Player_Data : public Char_Data
{
 public:
  char_data*     switched;
  char_data*     familiar;
  int            base_age;
  int                bank;
  int              prayer;
  int          gossip_pts;
  int             whistle;
  int            timezone;
  int            wizinvis;
  int           noteboard;
  note_data*    note_edit;
  int               iflag  [ 2 ];

  alias_array       alias;
  content_array    locker;
  content_array    junked;
  obj_array     save_list;
  reput_data   reputation;

  tell_data*        chant;
  tell_data*          say;
  tell_data*         yell;
  tell_data*        shout;
  tell_data*         tell;
  tell_data*        gtell;
  tell_data*         chat;
  tell_data*       gossip;
  tell_data*        ctell;
  tell_data*      whisper;
  tell_data*        atalk;
  tell_data*           to;

  Player_Data( char* );
  virtual ~Player_Data( );

  int           Type            ( )  { return PLAYER_DATA; }
  bool          Is_Valid        ( )  { return valid == PLAYER_DATA; }

  int           Age             ( );
  const char*   Location        ( content_array* = NULL );
  int           Size            ( ); 
  int           Light           ( int ); 

  float          Mean_Hp         ( );
  float          Mean_Mana       ( ); 
  float          Mean_Move       ( );

  bool           Accept_Msg      ( char_data* );
};


/*
 *   WIZARD CLASS
 */


class Wizard_Data : public player_data
{
 public:
  action_data*     action_edit;
  player_data*     player_edit;
  exit_data*         exit_edit;
  extra_data*       adata_edit;
  extra_data*      mpdata_edit;
  extra_data*      opdata_edit;
  extra_data*      oextra_edit;
  extra_data*        room_edit;
  mprog_data*       mprog_edit;
  obj_clss_data*      obj_edit;
  oprog_data*       oprog_edit;
  quest_data*       quest_edit;
  species_data*       mob_edit;
  int              custom_edit;
  int              rtable_edit;
  int                list_edit;
  int               table_edit  [ 2 ];

  int               permission  [ 2 ];
  int                   office;

  char*                 bamfin;
  char*                bamfout;
  char*            level_title;

  tell_data*        build_chan;
  tell_data*          imm_talk;
  tell_data*          god_talk;
  tell_data*            avatar;

  Wizard_Data( char* );
  virtual ~Wizard_Data( );

  virtual int   Type            ( )  { return WIZARD_DATA; }
  bool          Is_Valid        ( )  { return valid == WIZARD_DATA; }

  bool          See_Account     ( pfile_data* );
};


/*
 *   MOB CLASS
 */


class Mob_Data : public Char_Data
{
 public:
  mob_data*          prev;
  trainer_data*  pTrainer;

  Mob_Data( ) {
    record_new( sizeof( Mob_Data ), MEM_MOBS );
    pTrainer  = NULL;
    valid     = MOB_DATA;
    mob_list += this;
    }

  ~Mob_Data( ) {
    record_delete( sizeof( Mob_Data ), MEM_MOBS );
    mob_list -= this;
    }

  int    Type          ( )  { return MOB_DATA; }
  bool   Is_Valid      ( )  { return valid == MOB_DATA; }
  int    Size          ( );
  int    Light         ( int ); 

  float          Mean_Hp         ( );
  float          Mean_Mana       ( ); 
  float          Mean_Move       ( );

  bool           Accept_Msg      ( char_data* );
};


/*
 *   DESCRIPTION DATA
 */


class Descr_Data
{
 public:
  char*           name;
  char*       keywords;
  char*       singular;
  char*         long_s;
  char*          adj_s;
  char*       prefix_s;
  char*         plural;
  char*         long_p;
  char*          adj_p;
  char*       prefix_p;
  char*       complete;

  Descr_Data( );
  ~Descr_Data( );
};


/*
 *   SHARE_DATA
 */


class Share_Data
{
 public:
  int       strength;
  int   intelligence;
  int         wisdom;
  int      dexterity;
  int   constitution;
  int          level;
  int      alignment;
  int           race;
  int          kills;
  int         deaths;
  int           fame;
  int         resist  [ MAX_RESIST ];
  int          skill  [ MAX_SKILL ];

  Share_Data( );
  ~Share_Data( );
}; 


/*
 *   SUPPORT FUNCTIONS
 */


inline time_t char_data :: time_played( )
{
  return current_time-logon+played;
};



















