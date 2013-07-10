/*
 *   AREA HEADER
 */


#define AREA_OPEN           0
#define AREA_WORTHLESS      1
#define AREA_ABANDONED      2
#define AREA_PROGRESSING    3
#define AREA_PENDING        4
#define AREA_BLANK          5
#define AREA_IMMORTAL       6
#define MAX_AREA_STATUS     7


#define MAX_AREA          200


extern  area_data*      area_list;
extern  const char*   area_status  [ MAX_AREA_STATUS ];

void    load_area      ( const char* );
void    load_rooms     ( FILE*, area_data* );
bool    save_area      ( area_data*, bool = FALSE );
bool    set_area_file  ( char_data*, char*, const char*& );


class Area_Data
{
 public:
  area_data*         next;
  room_data*   room_first;
  char*              file;
  const char*     creator;
  const char*        name;
  const char*        help;
  bool           modified;
  int             nplayer;
  int               level;
  int          reset_time;
  int                 age;
  int              status;
  int               flags;

  Area_Data( );
};


room_data*   get_room_index    ( int, bool = FALSE );
bool         can_edit          ( char_data*, room_data*, bool = TRUE );


/*
 *   EXIT HEADER
 */


#define MAX_DOOR                    6


#define EX_ISDOOR                   0
#define EX_CLOSED                   1
#define EX_LOCKED                   2
#define EX_SECRET                   3
#define EX_PICKPROOF                4
#define EX_NO_SHOW                  5
#define EX_NO_OPEN                  6
#define EX_RESET_CLOSED             7      
#define EX_RESET_LOCKED             8
#define EX_RESET_OPEN               9
#define EX_REQUIRES_CLIMB          10
#define EX_SEARCHABLE              11
#define MAX_DFLAG                  12


extern const char* open_msg  [];
extern const char* close_msg [];
extern const char* lock_door_msg [];
extern const char* unlock_door_msg [];
extern const char* knock_door_msg [];


class Exit_Data : public Visible_Data
{
 public:
  room_data*    to_room;
  int           exit_info;
  int           key;
  int           strength;
  int           size;
  int           direction;
  int           light;
  const char*   name;
  const char*   keywords;

  Exit_Data     ( );
  ~Exit_Data    ( );

  virtual int Type( )  { return EXIT_DATA; }

  virtual const char*   Name       ( char_data*, int = 1, bool = FALSE );
  virtual const char*   Seen_Name  ( char_data*, int = 1, bool = FALSE );
  virtual const char*   Keywords   ( char_data* );
  virtual bool          Seen       ( char_data* );
  void                  Look_At    ( char_data* );
};


int         exits_prompt       ( char*, char_data* );
void        read_exits         ( FILE*, room_data*, int );
void        autoexit           ( char_data* );

exit_data*  exit_direction     ( room_data*, int );
exit_data*  reverse            ( exit_data* );

int         find_door          ( char_data*, char*, bool = TRUE );
int         find_door_always   ( char_data*, char* );

exit_data*  random_exit          ( room_data* );
exit_data*  random_open_exit     ( room_data* );
exit_data*  random_movable_exit  ( char_data* );

bool        open_door          ( char_data*, exit_data* );
bool        close_door         ( char_data*, exit_data* );
bool        lock_door          ( char_data*, exit_data* );
bool        unlock_door        ( char_data*, exit_data* );
bool        pick_door          ( char_data*, exit_data* );
void        bash_door          ( char_data*, exit_data* );
void        knock_door         ( char_data*, exit_data* );


/*
 *   LOCATIONS TYPES
 */


#define LOC_INDOORS     0
#define LOC_OUTDOORS    1
#define LOC_SUNLIGHT    2
#define LOC_FULLMOON    3
#define LOC_FOREST      4
#define LOC_UNDERWATER  5
#define MAX_LOCATION    6


bool allowed_location   ( char_data*, int*, const char*, const char* );


extern flag_data location_flags;


/*
 *   ROOM HEADER
 */


#define ROOM_LIMBO                  2
#define ROOM_CHAT                   9
#define ROOM_BEGIN                904 
#define ROOM_TEMPLE               904
#define ROOM_ALTAR                904
#define ROOM_CHIIRON_TEMPLE       904
#define ROOM_MEDIENNE_TEMPLE      101
#define ROOM_LLEWYRR_TEMPLE     19040
#define ROOM_DWARF_TEMPLE       27017
#define ROOM_DARKHAVEN_ALTAR    30010
#define ROOM_GNOME_CHURCH       33558
#define ROOM_PRISON                13
#define ROOM_DEATH              33751
#define ROOM_CRASH              33752


#define RFLAG_LIT                   0
#define RFLAG_SAFE                  1
#define RFLAG_INDOORS               2
#define RFLAG_NO_MOB                3
#define RFLAG_NO_RECALL             4
#define RFLAG_NO_MAGIC              5
#define RFLAG_NO_AUTOSCAN           6
#define RFLAG_ALTAR                 7
#define RFLAG_ALTAR_GOOD            8
#define RFLAG_ALTAR_NEUTRAL         9
#define RFLAG_ALTAR_EVIL           10
#define RFLAG_BANK                 11
#define RFLAG_SHOP                 12
#define RFLAG_PET_SHOP             13
#define RFLAG_OFFICE               14
#define RFLAG_NO_PRAY              15
#define RFLAG_SAVE_ITEMS           16
#define RFLAG_UNDERGROUND          17
#define RFLAG_AUCTION_HOUSE        18
#define RFLAG_RESET0               19
#define RFLAG_RESET1               20
#define RFLAG_RESET2               21
#define RFLAG_STATUS0              22
#define RFLAG_STATUS1              23
#define RFLAG_STATUS2              24
#define RFLAG_NO_MOUNT             25
#define RFLAG_ARENA                26
#define RFLAG_DONATION             27
#define RFLAG_NO_PKILL             28
#define MAX_RFLAG                  29


class Room_Data : public Thing_Data
{
 public:
  exit_array           exits; 
  extra_array    extra_descr;
  room_data*            next;
  room_data*       next_list;
  area_data*            area;
  action_data*        action;
  reset_data*          reset;
  track_data*          track;
  char*                 name;
  char*          description;
  char*             comments;
  int                   vnum;
  int             room_flags;
  int            sector_type;
  int               distance;
  int                counter;
  int                   size;

  Room_Data( ) {
    record_new( sizeof( room_data ), MEM_ROOM );
    track        = NULL;
    action       = NULL;
    area         = NULL;
    reset        = NULL;
    vnum         = -1;
    name         = empty_string;
    description  = empty_string;
    comments     = empty_string;
    room_flags   = 0;
    sector_type  = 0;
    size         = SIZE_DINOSAUR;
    distance     = MAX_INTEGER;
    }

  virtual ~Room_Data( ) {
    record_delete( sizeof( room_data ), MEM_ROOM );
    free_string( name, MEM_ROOM );
    free_string( description, MEM_ROOM );
    free_string( comments, MEM_ROOM );
    }

  virtual int Type    ( )        { return ROOM_DATA; }
          int Light   ( int );
  
  const char*  Location   ( content_array* = NULL );
  bool         is_dark    ( );
  bool         Seen       ( char_data* );
};


void  save_room_items  ( room_data* );
void  load_room_items  ( void );


/*
 *   TERRAIN HEADER
 */


#define SECT_INSIDE                 0  
#define SECT_CITY                   1
#define SECT_FIELD                  2
#define SECT_FOREST                 3
#define SECT_HILLS                  4
#define SECT_MOUNTAIN               5
#define SECT_WATER_SURFACE          6
#define SECT_UNDERWATER             7
#define SECT_RIVER                  8
#define SECT_AIR                    9
#define SECT_DESERT                10
#define SECT_CAVES                 11
#define SECT_ROAD                  12
#define SECT_SHALLOWS              13
#define MAX_TERRAIN                14


class Terrain_Type 
{
 public:
   const char*            name;
   int           movement_cost;
};


extern const class terrain_type terrain  [ MAX_TERRAIN ];


bool   water_logged   ( room_data* );
bool   deep_water     ( room_data* );
bool   is_submerged   ( char_data* );


