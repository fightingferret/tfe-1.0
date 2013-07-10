/* 
 *   OBJ_DATA CLASS
 */


#define ANTI_HUMAN                  0
#define ANTI_ELF                    1
#define ANTI_GNOME                  2
#define ANTI_DWARF                  3
#define ANTI_HALFLING               4
#define ANTI_ENT                    5
#define ANTI_CENTAUR                6
#define ANTI_LIZARD                 7
#define ANTI_OGRE                   8
#define ANTI_TROLL                  9
#define ANTI_ORC                   10
#define ANTI_GOBLIN                11
#define ANTI_VYAN                  12
#define ANTI_MAGE                  14
#define ANTI_PSIONIC               20
#define ANTI_MONK                  21
#define ANTI_UNUSED1               23
#define ANTI_UNUSED2               24
#define ANTI_UNUSED3               25
#define ANTI_GOOD                  26
#define ANTI_NEUTRAL               27
#define ANTI_EVIL                  28
#define ANTI_LAWFUL                29
#define ANTI_CHAOTIC               31
#define MAX_ANTI                   32


#define ITEM_TAKE                   0
#define ITEM_WEAR_FINGER            1
#define ITEM_WEAR_NECK              2
#define ITEM_WEAR_BODY              3
#define ITEM_WEAR_HEAD              4
#define ITEM_WEAR_LEGS              5
#define ITEM_WEAR_FEET              6
#define ITEM_WEAR_HANDS             7 
#define ITEM_WEAR_ARMS              8
#define ITEM_UNUSED2                9
#define ITEM_UNUSED3               10 
#define ITEM_WEAR_WAIST            11
#define ITEM_WEAR_WRIST            12
#define ITEM_HELD_R                13
#define ITEM_HELD_L                14
#define ITEM_WEAR_UNUSED0          15 
#define ITEM_WEAR_UNUSED1          16
#define ITEM_WEAR_UNUSED2          17
#define ITEM_WEAR_UNUSED3          18
#define ITEM_WEAR_HORSE_BODY       19
#define ITEM_WEAR_HORSE_BACK       20
#define ITEM_WEAR_HORSE_FEET       21      
#define MAX_ITEM_WEAR              22

#define ITEM_LIGHT                  1
#define ITEM_SCROLL                 2
#define ITEM_WAND                   3
#define ITEM_STAFF                  4
#define ITEM_WEAPON                 5
#define ITEM_GEM                    6
#define ITEM_SPELLBOOK              7
#define ITEM_TREASURE               8
#define ITEM_ARMOR                  9
#define ITEM_POTION                10
#define ITEM_REAGENT               11 
#define ITEM_FURNITURE             12
#define ITEM_TRASH                 13
#define ITEM_CROSS                 14 
#define ITEM_CONTAINER             15
#define ITEM_LOCK_PICK             16 
#define ITEM_DRINK_CON             17
#define ITEM_KEY                   18
#define ITEM_FOOD                  19
#define ITEM_MONEY                 20
#define ITEM_KEYRING               21
#define ITEM_BOAT                  22
#define ITEM_CORPSE                23
#define ITEM_UNUSED                24
#define ITEM_FOUNTAIN              25
#define ITEM_WHISTLE               26
#define ITEM_TRAP                  27
#define ITEM_LIGHT_PERM            28
#define ITEM_BANDAGE               29
#define ITEM_BOUNTY                30
#define ITEM_GATE                  31
#define ITEM_ARROW                 32 
#define ITEM_SKIN                  33
#define ITEM_BODY_PART             34
#define ITEM_CHAIR                 35
#define ITEM_TABLE                 36
#define ITEM_BOOK                  37
#define ITEM_PIPE                  38
#define ITEM_TOBACCO               39
#define ITEM_DECK_CARDS            40
#define ITEM_FIRE                  41
#define ITEM_GARROTE               42
#define MAX_ITEM                   43


#define MAT_PAPER                   0
#define MAT_WOOD                    1
#define MAT_LEATHER                 2
#define MAT_UNUSED1                 3
#define MAT_CLOTH                   4
#define MAT_GLASS                   5
#define MAT_STONE                   6
#define MAT_BONE                    7
#define MAT_FLESH                   8
#define MAT_ORGANIC                 9
#define MAT_UNUSED2                10
#define MAT_BRONZE                 11
#define MAT_IRON                   12
#define MAT_STEEL                  13
#define MAT_MITHRIL                14
#define MAT_ADAMANTINE             15
#define MAT_ELECTRUM               16
#define MAT_SILVER                 17
#define MAT_GOLD                   18
#define MAT_COPPER                 19
#define MAT_PLATINUM               20
#define MAT_KRYNITE                21 
#define MAX_MATERIAL               22

 
#define OBJ_COPPER                 12
#define OBJ_SILVER                 14
#define OBJ_GOLD                    2
#define OBJ_PLATINUM               16
#define OBJ_CORPSE_NPC             10
#define OBJ_CORPSE_PC              11
#define OBJ_BALL_OF_LIGHT          21
#define OBJ_CROSS                 109
#define OBJ_LOCK_PICK             127
#define OBJ_ASTRAL_GATE           270
#define OBJ_GLASS_VIAL            389
#define OBJ_RECALL               1021
#define OBJ_COLLAR               2141
#define OBJ_DAGGER               3208


#define OFLAG_GLOW                  0
#define OFLAG_HUM                   1
#define OFLAG_DARK                  2
#define OFLAG_LOCK                  3
#define OFLAG_EVIL                  4
#define OFLAG_IS_INVIS              5
#define OFLAG_MAGIC                 6
#define OFLAG_NODROP                7
#define OFLAG_SANCT                 8
#define OFLAG_FLAMING               9
#define OFLAG_BACKSTAB             10
#define OFLAG_NO_DISARM            11
#define OFLAG_NOREMOVE             12
#define OFLAG_INVENTORY            13
#define OFLAG_NO_SHIELD            14
#define OFLAG_NO_MAJOR             15
#define OFLAG_NOSHOW               16
#define OFLAG_NOSACRIFICE          17
#define OFLAG_WATER_PROOF          18
#define OFLAG_APPRAISED            19
#define OFLAG_NO_SELL              20
#define OFLAG_NO_JUNK              21
#define OFLAG_IDENTIFIED           22
#define OFLAG_RUST_PROOF           23
#define OFLAG_BODY_PART            24 
#define OFLAG_CHAIR                25
#define OFLAG_NOSAVE               26
#define OFLAG_BURNING              27
#define OFLAG_ADDITIVE             28
#define OFLAG_GOOD                 29
#define OFLAG_THE                  30
#define OFLAG_REPLICATE            31
#define OFLAG_KNOWN_LIQUID         32
#define OFLAG_POISON_COATED        33
#define OFLAG_NO_AUCTION           34
#define OFLAG_NO_ENCHANT           35
#define OFLAG_COPIED               36
#define OFLAG_RANDOM_METAL         37
#define OFLAG_COVER                38
#define MAX_OFLAG                  39

#define RESTR_BLADED                0
#define RESTR_NO_HIDE               1
#define RESTR_NO_SNEAK              2
#define RESTR_DISHONORABLE          3
#define MAX_RESTRICTION             4


/*
 *   OBJ_CLSS_DATA 
 */


extern obj_clss_data* obj_index_list [ MAX_OBJ_INDEX ];


class Obj_Clss_Data
{
 public:
  affect_array        affected;
  extra_array      extra_descr;
  oprog_data*            oprog;
  char*               singular;
  char*                 plural;
  char*                 before;
  char*                  after;
  char*        prefix_singular;
  char*          prefix_plural;
  char*                 long_s;  
  char*                 long_p;
  char*                creator;
  char*               last_mod;
  time_t                  date;
  int                     vnum;
  int                    fakes;
  int                item_type;
  int              extra_flags  [ 2 ];
  int               size_flags;
  int             restrictions;
  int               anti_flags;
  int                materials;
  int               wear_flags;
  int              layer_flags;
  int             affect_flags  [ AFFECT_INTS ];
  int                    count;
  int                    limit;
  int                   weight;
  int                     cost;
  int                    level;
  int                    value  [ 4 ];
  int                   repair;
  int               durability;   
  int                   blocks;
  int                    light;

  Obj_Clss_Data   ( );
  ~Obj_Clss_Data  ( );

  const char*  Name      ( );
  const char*  Name      ( int );
  const char*  Keywords  ( );

  int          metal     ( );
};


bool   can_extract    ( obj_clss_data*, char_data* );
void   load_objects   ( void );
void   junk_obj       ( char_data*, obj_data* );

char*        name_before   ( obj_clss_data* );
char*        name_brief    ( obj_clss_data*, int = 1 );
const char*  after_descr   ( obj_clss_data* );
const char*  before_descr  ( obj_clss_data* );


/*
 *   OBJ_DATA 
 */


extern obj_array obj_list;


class Obj_Data : public Thing_Data
{
 public :
  affect_array         affected;

  extra_data*       extra_descr;
  obj_clss_data*     pIndexData;
  player_data*             save;
  char*                  source;
  char*                   label;
  char*                singular;
  char*                  plural;
  char*                  before;
  char*                   after;
  pfile_data*             owner;
  int               extra_flags  [ 2 ];
  int                size_flags;
  int                 materials;
  int                    weight;
  int                     value  [ 4 ];
  int                     timer;
  int                 condition;
  int                      rust;
  int                       age;  
  int                     layer;

  Obj_Data( obj_clss_data* );
  virtual ~Obj_Data( );

  /* BASIC */

  int   Type     ( )       { return OBJ_DATA; }
  void  Extract  ( );
  void  Extract  ( int );

  /* NAME/KEYWORDS */

  const char*  Keywords        ( char_data* );
  const char*  Name            ( char_data* = NULL, int = 1, bool = FALSE );
  const char*  Seen_Name       ( char_data*, int = 1, bool = FALSE );
  const char*  Show            ( char_data*, int = 1 );
  const char*  condition_name  ( char_data*, bool = FALSE );
  const char*  Location        ( content_array* = NULL );
  void         Look_At         ( char_data* ); 
  bool         Seen            ( char_data* );
 
  /* TO/FROM */

  void         To              ( content_array* );
  void         To              ( thing_data* );
  thing_data*  From            ( int = 1 );

  /* PROPERTIES */

  int          Weight          ( int = -1 );
  int          Light           ( int = -1 );
  int          Empty_Weight    ( int = -1 );
  int          Number          ( int = -1 );
  int          Capacity        ( );
  int          Empty_Capacity  ( );
  bool         Damaged         ( );
  int          Cost            ( );
  bool         Belongs         ( char_data* );

  bool droppable( ) { return !is_set( extra_flags, OFLAG_NODROP ); }
  bool removable( ) { return !is_set( extra_flags, OFLAG_NOREMOVE ); } 
  bool paper( )     { return  is_set( &materials, MAT_PAPER ); }
  bool glass( )     { return  is_set( &materials, MAT_GLASS ); }
  bool wood( )      { return  is_set( &materials, MAT_WOOD ); }
  bool stone( )     { return  is_set( &materials, MAT_STONE ); }

  int  metal           ( ); 
  bool is_cloth        ( );
  bool is_leather      ( );

  /* SAVING THROWS */

  int vs_fire ( );
  int vs_acid ( );
  int vs_cold ( );
};


/*
 *   FUNCTIONS
 */


obj_data*    create             ( obj_clss_data*, int = 1 );
obj_data*    duplicate          ( obj_data*, int = 1 );

void         set_owner          ( obj_data*, char_data*, char_data* );
void         set_owner          ( obj_data*, pfile_data* );
void         set_owner          ( pfile_data*, thing_array& );
  
bool         read_object        ( FILE*, content_array&, char* );
void         write_object       ( FILE*, content_array& );

bool         is_same            ( obj_data*, obj_data* );

int          armor_class        ( obj_data* );
void         rust_object        ( obj_data*, int );
void         condition_abbrev   ( char*, obj_data*, char_data* );
void         age_abbrev         ( char*, obj_data*, char_data* );
bool         newbie_abuse       ( char_data* );
const char*  light_name         ( int );

void         open_object        ( char_data*, obj_data* );
void         close_object       ( char_data*, obj_data* );


/*
 *   OBJECT ARGUMENTS
 */


obj_data*    find_type         ( thing_array&, int ); 
obj_data*    find_vnum         ( thing_array&, int ); 


/*
 *   ARRAY ROUTINES
 */


void         rehash             ( char_data*, thing_array& );
void         rehash_weight      ( char_data*, thing_array& );
void         sort_objects       ( char_data*, thing_array&, thing_data*, int,
                                  thing_array*, thing_func** );

void         page_publ          ( char_data*, thing_array*, const char*,
                                  thing_data* = NULL,
                                  const char* = empty_string,
                                  const char* = empty_string );
void         page_priv          ( char_data*, thing_array*, const char*,
                                  thing_data* = NULL,
                                  const char* = empty_string,
                                  const char* = empty_string );
void         send_priv          ( char_data*, thing_array*, const char*,
                                  thing_data* );
void         send_publ          ( char_data*, thing_array*, const char*,
                                  const char* );

thing_func      stolen;
thing_func      corpse;
thing_func      cursed;
thing_func        same;
thing_func    wont_fit;
thing_func   cant_take;
thing_func      sat_on;
thing_func        many;
thing_func       heavy;
thing_func   forbidden;
thing_func      gotten;
thing_func         put;
thing_func        drop;
thing_func       paper;
thing_func       glass;


extern const char* unlock_msg [];
extern const char* lock_msg [];


/*
 *   INLINE UTILITIES
 */


inline char_data* carried_by( obj_data* )
{
  return NULL;
}


inline obj_clss_data* get_obj_index( int vnum )
{
  if( vnum < 0 || vnum >= MAX_OBJ_INDEX ) 
    return NULL;

  return obj_index_list[vnum];
}


inline int repair_condition( obj_data* obj )
{
  return obj->pIndexData->durability-5*obj->age;
};


/*
 *   FOOD ROUTINES
 */


extern const char* cook_word [];


bool eat    ( char_data*, obj_data* );
bool drink  ( char_data*, obj_data* );


/*
 *   NAME ROUTINES
 */


extern bool include_empty;
extern bool include_liquid;
extern bool include_closed;


/*
 *   OBJECT TRANSFER ROUTINES
 */


void        drop_contents      ( obj_data* );


/*
 *   MONEY ROUTINES
 */


int   monetary_value    ( obj_data* );
bool  remove_silver     ( char_data* );
void  split_money       ( char_data*, int, bool );
void  add_coins         ( char_data*, int, char* = NULL );


/*
 *   WEIGHT FUNCTIONS
 */


bool   can_carry         ( char_data*, obj_data*, bool = TRUE );

