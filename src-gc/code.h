/*
 *   TYPE DECLARATIONS
 */


typedef class Loop_Type   loop_type;
typedef class Aif_Type    aif_type;
typedef class Afunc_Type  afunc_type;
typedef class Cfunc_Type  cfunc_type;


enum arg_enum {  NONE, ANY, STRING, INTEGER, CHARACTER,
  OBJECT, ROOM, DIRECTION, NATION, SKILL, RFLAG, STAT, CLASS,
  RELIGION, RACE, THING };
 
enum fam_enum  { variable, constant, function, if_clause, end, cont, loop };
enum op_enum   { greater_than, less_than, is_equal, set_equal, not_equal };
enum loop_enum { loop_all_in_room, loop_followers, loop_unknown }; 


typedef void* cfunc     ( void** );


/*
 *   QUEUE CLASS
 */


class Queue_Data
{
 public:
  queue_data*        next;
  arg_type*           arg;
  char_data*           ch;
  char_data*          mob;
  obj_data*           obj;
  room_data*         room;
  int                   i;
  program_data*   program;
  int                time;

  Queue_Data( ) {
    record_new( sizeof( queue_data ), MEM_QUEUE );
    return;
    }

  ~Queue_Data( ) {
    record_delete( sizeof( queue_data ), MEM_QUEUE );
    return;
    }
};


/*
 *   ARGUMENT TYPES
 */


class Arg_Type
{
 public:
  int           type;
  int         family;
  void*        value;
  arg_type*     next;
  bool           neg;

  Arg_Type( ) {
    record_new( sizeof( arg_type ), MEM_PROGRAM );
    next  = NULL;
    neg   = FALSE;
    value = NULL;
    type  = NONE;
    };

  ~Arg_Type( ) {
    record_delete( sizeof( arg_type ), MEM_PROGRAM );
    if( next != NULL )
      delete next;
    };
};


class Loop_Type : public arg_type
{
 public:
  loop_enum       fruit;
  arg_type*       aloop;
  arg_type*   condition;

  Loop_Type( ) {
    record_new( sizeof( loop_type ), MEM_PROGRAM );
    family    = loop;
    aloop     = NULL;
    condition = NULL;
    return;
    }

  ~Loop_Type( ) {
    record_delete( sizeof( loop_type ), MEM_PROGRAM );
    if( aloop != NULL ) 
      delete aloop;
    if( condition != NULL )
      delete condition;
    return;
    }
};


class Aif_Type : public arg_type
{
  public:
    arg_type*  condition;
    arg_type*        yes;
    arg_type*         no;        

  Aif_Type( ) {
    record_new( sizeof( aif_type ), MEM_PROGRAM );
    family    = if_clause;
    condition = NULL;
    yes       = NULL;
    no        = NULL;
    return;
    }

  ~Aif_Type( ) {
    record_delete( sizeof( aif_type ), MEM_PROGRAM );
    if( condition != NULL )
      delete condition;
    if( yes != NULL )
      delete yes;
    if( no != NULL )
      delete no;
    return;
    }
};


class Afunc_Type : public arg_type
{
  public:
    const cfunc_type*  func;
    arg_type*           arg  [4];

  Afunc_Type( ) {
    record_new( sizeof( afunc_type ), MEM_PROGRAM );
    family = function;
    vzero( arg, 4 ); 
    return;
    }

  ~Afunc_Type( ) {
    int i;
    record_delete( sizeof( afunc_type ), MEM_PROGRAM );
    for( i = 0; i < 4; i++ )
      if( arg[i] != NULL )
        delete arg[i];
    return;
    }
};


/*
 *   ???
 */


class Cfunc_Type
{
  public:
    char*     name;
    cfunc*    func_call;
    arg_enum  type;
    arg_enum  arg[4];
};


class Stack_Data
{
 public:
  stack_data*    next;
  char_data*       ch;
  char_data*   victim;
  obj_data*       obj;
  room_data*     room;
  char_data*      mob;
  char_data*      rch;
  int               i;
};


/*
 *   CODE RELATED FUNCTIONS
 */


void*        code_alloc      ( int );
char*        code_alloc      ( const char* );
void         code_bug        ( char* );
void         pop             ( void );
void         push            ( void );
void         compile         ( program_data* );
char*        get_string      ( const char*, extra_array& );
void         clear_queue     ( program_data* );


arg_type*    read_arg        ( );
arg_type*    read_op         ( arg_type* );
arg_type*    read_string     ( const char*&, extra_array& );
arg_type*    read_variable   ( const char*&, bool );
aif_type*    read_if         ( const char*& );
loop_type*   read_loop       ( const char*& );
arg_type*    read_function   ( const char*&, bool );
arg_type*    read_const      ( const char*& );
arg_type*    read_digit      ( const char*& );


extern bool               end_prog;
extern bool              cont_prog;
extern bool             queue_prog;
extern queue_data*      queue_list;
extern arg_type*          curr_arg;
extern char              error_buf  [ MAX_INPUT_LENGTH ];
extern mem_block*       block_list;
extern const char*   arg_type_name  [];

extern const cfunc_type cfunc_list [];








