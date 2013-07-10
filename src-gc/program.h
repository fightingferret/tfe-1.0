/*
 *   BASE PROGRAM CLASS
 */


const char*   prog_msg       ( program_data*, const char*, const char* );
bool          execute        ( program_data* );
void          show_defaults  ( char_data*, int, const char*** );


class Program_Data
{
 public:
  char*            code;
  extra_array      data;
  arg_type*      binary;
  mem_block*     memory;
  int            active;
  bool          corrupt;

  Program_Data( ) {
    record_new( sizeof( program_data ), MEM_PROGRAM );
    code    = empty_string;
    binary  = NULL;
    memory  = NULL;
    active  = 0;
    corrupt = FALSE;
    }

  virtual ~Program_Data( ) {
    record_delete( sizeof( program_data ), MEM_PROGRAM );
    free_string( code, MEM_PROGRAM );
    delete_list( memory );
    }

  virtual void display( char_data* ) {
    return;
    }
};


/*
 *   ACTION HEADER
 */


#define TRIGGER_NONE                0
#define TRIGGER_ENTERING            1
#define TRIGGER_RANDOM              2
#define TRIGGER_LEAVING             3
#define TRIGGER_RANDOM_ALWAYS       4
#define TRIGGER_SACRIFICE           5
#define TRIGGER_TIME                6
#define TRIGGER_ATTACK              7
#define TRIGGER_OPEN_DOOR           8
#define TRIGGER_SEARCHING           9
#define TRIGGER_CLOSE_DOOR         10
#define TRIGGER_UNLOCK_DOOR        11
#define TRIGGER_LOCK_DOOR          12
#define TRIGGER_KNOCK_DOOR         13
#define MAX_ATN_TRIGGER            14


void   action_update       ( void );
bool   check_actions       ( char_data*, char*, char* );    
bool   check_action_flags  ( char_data*, int, bool = TRUE );


class Action_Data : public program_data
{
 public:
  action_data*      next;
  int            trigger;
  int              value;
  int              flags;
  const char*    command;
  const char*     target;  
  room_data*        room;

  Action_Data( ) {
    record_new( sizeof( action_data ), MEM_ACTION );
    command = empty_string;
    target  = empty_string;
    trigger = 0;
    value   = 0;
    flags   = 0;
    room    = NULL;
    next    = NULL;
    };

  ~Action_Data( ) {
    record_delete( sizeof( action_data ), MEM_ACTION );
    free_string( command, MEM_ACTION );
    free_string( target, MEM_ACTION );
    };

         void   display        ( char_data* );
  friend void   read           ( FILE*, action_data*&, room_data* );
  friend void   write          ( FILE*, action_data* ); 
};


/* 
 *   MPROG HEADER
 */


#define MPROG_TRIGGER_ENTRY         0
#define MPROG_TRIGGER_LEAVING       1
#define MPROG_TRIGGER_ASKING        2
#define MPROG_TRIGGER_BLOCK         3
#define MPROG_TRIGGER_DEATH         4
#define MPROG_TRIGGER_KILL          5
#define MPROG_TRIGGER_GIVE          6
#define MPROG_TRIGGER_RESET         7
#define MPROG_TRIGGER_TELL          8
#define MPROG_TRIGGER_SKIN          9
#define MPROG_TRIGGER_TIMER        10
#define MPROG_TRIGGER_ATTACK       11
#define MPROG_TRIGGER_ORDER        12
#define MAX_MPROG_TRIGGER          13


void extract   ( mprog_data*, wizard_data* );


class Mprog_Data : public program_data
{
 public:
  mprog_data*        next;
  int             trigger;
  int               value;
  char*            string;
  species_data*   species;

  Mprog_Data( ) {
    record_new( sizeof( mprog_data ), MEM_MPROG );
    species = NULL;
    return;
    }

  ~Mprog_Data( ) {
    record_delete( sizeof( mprog_data ), MEM_MPROG );
    return;
    }

  void display( char_data* );
};


/*
 *   OBJECT PROGRAM HEADER
 */


#define OPROG_TRIGGER_PUT           0
#define OPROG_TRIGGER_GET           1
#define OPROG_TRIGGER_TIMER         2
#define OPROG_TRIGGER_HIT           3
#define OPROG_TRIGGER_NONE          4
#define OPROG_TRIGGER_TO_ROOM       5
#define OPROG_TRIGGER_ENTERING      6
#define OPROG_TRIGGER_WEAR          7
#define OPROG_TRIGGER_CONSUME       8
#define OPROG_TRIGGER_SIT           9
#define OPROG_TRIGGER_RANDOM       10
#define OPROG_TRIGGER_DISARM       11
#define OPROG_TRIGGER_USE          12
#define OPROG_TRIGGER_UNLOCK       13
#define OPROG_TRIGGER_LOCK         14
#define MAX_OPROG_TRIGGER          15


class Oprog_Data : public program_data
{
 public:
  oprog_data*          next;
  obj_clss_data*    obj_act;
  int              obj_vnum;
  int               trigger;
  char*              target;
  char*             command;
  int                 value;
  obj_clss_data*   obj_clss;

  Oprog_Data( ) {
    record_new( sizeof( oprog_data ), MEM_OPROG );
    obj_act    = NULL;
    obj_vnum   = 0;
    trigger    = 0;
    value      = -1;
    target     = empty_string;
    command    = empty_string;
    obj_clss   = NULL;
    next       = NULL;
    };

  ~Oprog_Data( ) {
    record_delete( sizeof( oprog_data ), MEM_OPROG );
    free_string( command, MEM_OPROG );
    free_string( target,  MEM_OPROG );
    }

  void display( char_data* );
};


/*
 *   SUPPORT FUNCTIONS
 */


bool   search_mload       ( arg_type*, int );
bool   search_oload       ( arg_type*, int );
bool   search_quest       ( arg_type*, int );











