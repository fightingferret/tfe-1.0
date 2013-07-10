#include "define.h"
#include "struct.h"


arg_type*     curr_arg;
mem_block*  block_list  = NULL;

char         error_buf  [ MAX_INPUT_LENGTH ];
char*             code;
extra_array*      data;


/*
 *   COMPILER ROUTINES
 */


void compile( program_data* program )
{
  arg_type*         arg;
  arg_type*    arg_list  = NULL;
  arg_type*    arg_last  = NULL;
  mem_block*       list;
  mem_block*   tmp_list;
  int              line;
  char*          letter;

  clear_queue( program );
  delete_list( program->memory );  

  if( program->binary != NULL ) {
    delete program->binary;
    program->binary = NULL;
    }
 
  program->memory = NULL;

  tmp_list   = block_list;
  code       = program->code;
  data       = &program->data;
  block_list = NULL;

  *error_buf = '\0';

  for( ; *error_buf == '\0'; ) {
    if( ( arg = read_op( read_arg( ) ) ) == NULL )
      break; 

    line += 2;
   
    if( *code == ';' )
      code++;

    if( arg_list == NULL )
      arg_list = arg;
    else
      arg_last->next = arg;

    arg_last = arg;
    }

  list       = block_list;
  block_list = tmp_list;

  if( *error_buf != '\0' ) { 
    for( line = 1, letter = program->code; letter != code; letter++ )
      if( *letter == '\n' )
        line += 2;
    program->corrupt = TRUE;
    page_header( var_ch, "*** %s\r\n\r\n", error_buf );
    page_header( var_ch, "*** Error on line %d\r\n", line );
    page_header( var_ch, "*** FAILS TO COMPILE\r\n" );
    delete_list( list );
    if( arg_list != NULL )
      delete arg_list;
    return;
    }

  program->binary  = arg_list;
  program->memory  = list;
  program->active  = 0;
  program->corrupt = FALSE;
}


/*
 *   READ_ARG
 */


arg_type* read_arg( )
{
  arg_type*    arg;
  arg_type*   arg1;
  arg_type*   arg2;
  bool         neg;

  if( *error_buf != '\0' )
    return NULL;

  skip_spaces( code );

  if( *code == '\0' ) 
    return NULL;

  if( *code == '{' ) {
    code++;
    if( ( arg1 = read_op( read_arg( ) ) ) == NULL )
      return NULL;
    for( arg2 = arg1; ; arg2 = arg2->next ) {
      skip_spaces( code );
      if( *code == '}' ) {
        code++;
        return arg1;
        }
      if( ( arg2->next = read_op( read_arg( ) ) ) == NULL ) {
        if( *error_buf == '\0' )
          strcpy( error_buf, "End of statement block without }." );
        delete arg1;
        return NULL;
        }
      }
    }       

  if( ( arg = read_const( code ) ) != NULL )
    return arg;
        
  if( exact_match( code, "end" ) ) {
    arg         = new arg_type;
    arg->family = end;
    return arg;
    }

  if( exact_match( code, "continue" ) ) {
    arg         = new arg_type;
    arg->family = cont;
    return arg;
    }

  if( exact_match( code, "loop(" ) )
    return read_loop( code );

  if( exact_match( code, "if(" ) ) 
    return read_if( code );

  if( neg = ( *code == '!' ) ) 
    code++;

  if( ( arg = read_function( code, neg ) ) != NULL 
    || *error_buf != '\0' )
    return arg;

  if( ( arg = read_variable( code, neg ) ) != NULL  
    || *error_buf != '\0' )
    return arg;

  if( neg )
    code--;

  if( isdigit( *code ) || ( *code == '-' && isdigit( *(code+1) ) ) ) 
    return read_digit( code );

  if( ( arg = read_string( code, *data ) ) != NULL 
    || *error_buf != '\0' )
    return arg;
  
  if( *error_buf == '\0' ) 
    strcpy( error_buf, "Error of unknown type." );

  return NULL;
}


/*
 *   OPERATORS
 */


cfunc_type op_list [ ] =
{
  { "=",     &code_set_equal,    NONE,    { INTEGER, INTEGER, NONE,  NONE }  },
  { "+=",    &code_plus_equal,   NONE,    { INTEGER, INTEGER, NONE,  NONE }  },
  { "-=",    &code_minus_equal,  NONE,    { INTEGER, INTEGER, NONE,  NONE }  },
  { "&&",    &code_and,          INTEGER, { INTEGER, INTEGER, NONE,  NONE }  },
  { "||",    &code_or,           INTEGER, { INTEGER, INTEGER, NONE,  NONE }  },
  { "==",    &code_is_equal,     INTEGER, { INTEGER, INTEGER, NONE,  NONE }  },
  { "!=",    &code_not_equal,    INTEGER, { INTEGER, INTEGER, NONE,  NONE }  },
  { ">",     &code_gt,           INTEGER, { INTEGER, INTEGER, NONE,  NONE }  },
  { "<",     &code_lt,           INTEGER, { INTEGER, INTEGER, NONE,  NONE }  },
  { ">=",    &code_ge,           INTEGER, { INTEGER, INTEGER, NONE,  NONE }  },
  { "<=",    &code_le,           INTEGER, { INTEGER, INTEGER, NONE,  NONE }  },
  { "",      NULL,               NONE,    { NONE,  NONE,  NONE,  NONE }  }
};


bool can_assign( arg_enum t1, arg_enum t2 )
{
  if( t1 == INTEGER )
    return TRUE;

  if( t1 == THING )
    return( t2 == CHARACTER || t2 == OBJECT || t2 == ROOM );

  return( t1 == t2 );  
}


arg_type* read_op( arg_type* arg )
{
  afunc_type*  afunc;
  int              i;

  if( arg == NULL || arg->family == if_clause 
    || arg->family == loop )
    return arg; 

  skip_spaces( code );

  if( *code == ')' || *code == ',' )
    return arg;

  if( *code == ';' ) {
    code++;
    return arg;
    }

  afunc         = new afunc_type;     
  afunc->type   = INTEGER;
  afunc->arg[0] = arg;

  for( i = 0; ; i++ ) {
    if( *op_list[i].name == '\0' ) {
      strcpy( error_buf, "Unknown operator or missing semi-colon." );
      delete afunc;
      return NULL;
      }
    if( exact_match( code, op_list[i].name ) ) 
      break;
    }

  afunc->func = &op_list[i];

  skip_spaces( code );

  if( ( afunc->arg[1] = read_arg( ) ) == NULL ) {
    if( *error_buf == '\0' )
      strcpy( error_buf, "Operator missing second argument." );
    delete afunc;
    return NULL;
    }

  if( i >= 3 && i <= 4 ) 
    return read_op( afunc );

  if( i >= 5 && i <= 6 ) {
    if( afunc->arg[1]->type != arg->type ) {
      sprintf( error_buf,
        "Both sides of operator '%s' must be of identical type.",
        op_list[i].name );
      delete afunc;
      return NULL;
      }
    return afunc;
    }

  if( i != 0 ) {
    if( afunc->arg[1]->type != INTEGER || arg->type != INTEGER ) {
      sprintf( error_buf,
        "Operator '%s' requires integer arguments.", op_list[i].name );
      delete afunc;
      return NULL; 
      }
    }

  if( i < 3 ) {
    if( arg->family != variable ) {
      strcpy( error_buf, "Assigning a value to a non-variable?" );
      delete afunc;
      return NULL;
      }
    if( !can_assign( (arg_enum) arg->type, (arg_enum) afunc->arg[1]->type ) ) {
      sprintf( error_buf, "Assignment to %s from %s.",
        arg_type_name[ arg->type ], arg_type_name[ afunc->arg[1]->type ] );
      delete afunc;
      return NULL;
      }
    if( *code != ';' ) {
      strcpy( error_buf, "Assignment missing trailing semi-colon." );
      delete afunc;
      return NULL;
      }
    code++;
    }

  return afunc;
}  


/*
 *   CONSTANTS
 */


class Const_Data
{
 public:
  const char**   entry1;
  const char**   entry2;
  int*             size;
  arg_enum         type;

  const char* entry( int j ) const {
    return *(entry1+j*(entry2-entry1));
    };
};


int max_clss   = MAX_CLSS;
int max_dir    = 6;
int max_relig  = MAX_RELIGION;
int max_rflag  = MAX_RFLAG;
int max_skill  = MAX_SKILL;
int max_stat   = 9;


const char* stat_name[] = { "str", "int", "wis", "dex", "con",
  "level", "piety", "class", "align" };


#define max_nation  MAX_ENTRY_NATION
#define max_race    MAX_ENTRY_RACE


const Const_Data const_list [] = 
{
  {  &nation_table[0].name, &nation_table[1].name, &max_nation, NATION     },
  {  &clss_table[0].name,   &clss_table[1].name,   &max_clss,   CLASS      },
  {  &dir_table[0].name,    &dir_table[1].name,    &max_dir,    DIRECTION  },
  {  &rflag_name[0],        &rflag_name[1],        &max_rflag,  RFLAG      },
  {  &skill_table[0].name,  &skill_table[1].name,  &max_skill,  SKILL      },
  {  &stat_name[0],         &stat_name[1],         &max_stat,   STAT       },
  {  &race_table[0].name,   &race_table[1].name,   &max_race,   RACE       },
  {  NULL,                  NULL,                  NULL,        NONE       },
};

#undef max_nation
#undef max_race


arg_type* read_const( const char*& code )
{
  arg_type*   arg;
  int        i, j;

  for( i = 0; const_list[i].entry1 != NULL; i++ ) {
    for( j = 0; j < *const_list[i].size; j++ ) {
      if( exact_match( code, const_list[i].entry( j ) ) ) {
        arg         = new arg_type;
        arg->type   = const_list[i].type;
        arg->family = constant;
        arg->value  = (void*) j;
        return arg;
        }
      }
    }

  return NULL;
}


/*
 *   VARIABLES
 */


class Var_Data
{
 public:
  char*        name;
  void*        pointer;
  arg_enum     type;  
};


var_data variable_list [] =
{
  { "mob",           &var_mob,            CHARACTER   }, 
  { "rch",           &var_rch,            CHARACTER   }, 
  { "victim",        &var_victim,         CHARACTER   }, 
  { "arg",           &var_arg,            STRING     },
  { "room",          &var_room,           ROOM },
  { "obj",           &var_obj,            OBJECT     },
  { "container",     &var_container,      OBJECT     },
  { "ch",            &var_ch,             CHARACTER   },
  { "i",             &var_i,              INTEGER   },
  { "j",             &var_j,              INTEGER   },
  { "",              NULL,                NONE    }
};


arg_type* read_variable( const char*& code, bool neg )
{
  arg_type*  arg;
  int          i;

  for( i = 0; ; i++ ) {
    if( *variable_list[i].name == '\0' ) 
      return NULL;
    if( exact_match( code, variable_list[i].name ) )
      break;
    }

  arg         = new arg_type;
  arg->type   = ( neg ? INTEGER : variable_list[i].type );
  arg->family = variable;
  arg->value  = (void*) variable_list[i].pointer;
  arg->neg    = neg;

  return arg;
}


/*
 *   LOOPS
 */


const char* loop_name[] = { "all_in_room", "followers" };


loop_type* read_loop( const char*& code )
{
  loop_type* aloop  = new loop_type;
  int            i;

  for( i = 0; i < 2; i++ ) {
    if( exact_match( code, loop_name[i] ) ) {
      if( *code != ')' ) {
        strcpy( error_buf, "Missing ')' after loop." ); 
        delete aloop;
        return NULL;
        }
      code++; 
      aloop->fruit = (loop_enum) i;
      if( ( aloop->aloop = read_arg( ) ) != NULL )
        return aloop;
      if( *error_buf == '\0' ) 
        strcpy( error_buf, "Error in loop." );
      delete aloop;
      return NULL;
      }
    }

  aloop->fruit = loop_unknown;
  aloop->neg   = ( *code == '!' );
  code        += aloop->neg;

  if( ( aloop->condition = read_op( read_arg( ) ) ) == NULL ) {
    if( *error_buf == '\0' ) 
      strcpy( error_buf, "Loop statement with null condition??" );
    delete aloop;
    return NULL;  
    }   

  skip_spaces( code );

  if( *code != ')' ) {
    strcpy( error_buf, "Loop statement missing closing )." );
    delete aloop;
    return NULL;
    }

  code++;

  if( ( aloop->aloop = read_arg( ) ) == NULL ) {
    if( *error_buf == '\0' ) 
      strcpy( error_buf, "Loop statement with null loop." );
    delete aloop;
    return NULL;  
    }   

  return aloop;
}  


/*
 *   IF STATEMENTS
 */


aif_type* read_if( const char*& code )
{
  aif_type*       aif  = new aif_type;
  const char*  letter;

  if( ( aif->condition = read_op( read_arg( ) ) ) == NULL ) {
    if( *error_buf == '\0' ) 
      strcpy( error_buf, "If statement with null condition??" );
    delete aif;
    return NULL;  
    }   

  skip_spaces( code );

  if( *code != ')' ) {
    strcpy( error_buf, "If statement missing closing )." );
    delete aif;
    return NULL;
    }

  code++;

  if( ( aif->yes = read_arg( ) ) == NULL 
    || ( aif->yes->next == NULL
    && ( aif->yes = read_op( aif->yes ) ) == NULL ) ) {
    if( *error_buf == '\0' )
      strcpy( error_buf, "If statement with no effect." );
    delete aif;
    return NULL;
    } 

  letter = code;
  for( ; isspace( *letter ) || *letter == ';'; letter++ );
  
  if( !strncasecmp( "else", letter, 4 ) ) {
    code = letter+4;
    if( ( aif->no = read_arg( ) ) == NULL 
      || ( aif->no->next == NULL
      && ( aif->no = read_op( aif->no ) ) == NULL ) ) {
      if( *error_buf == '\0' )
        strcpy( error_buf, "Else statement with no effect." );
      delete aif;
      return NULL;
      }
    }
 
  return aif;
}


/*
 *   FUNCTIONS
 */


arg_type* read_function( const char*& code, bool neg )
{
  afunc_type*  afunc;
  int           i, j;
  int         length;

  for( i = 0; ; i++ ) {
    if( cfunc_list[i].name[0] == '\0' ) 
      return NULL;
    length = strlen( cfunc_list[i].name );
    if( !strncasecmp( cfunc_list[i].name, code, length ) 
      && code[length] == '(' ) 
      break;
    }

  code       += length+1; 
  afunc       = new afunc_type;     
  afunc->type = ( neg ? INTEGER : cfunc_list[i].type );
  afunc->neg  = neg;
  afunc->func = &cfunc_list[i];

  for( j = 0; ; j++ ) {
    skip_spaces( code );
    if( *code == ')' ) 
      break;
    if( j == 4 ) {
      sprintf( error_buf,
        "Too many arguments for function %s.", cfunc_list[i].name );
      delete afunc;
      return NULL;
      }
    if( ( afunc->arg[j] = read_op( read_arg( ) ) ) == NULL ) {
      if( *error_buf == '\0' )
        strcpy( error_buf, "Function missing closing ')'." );
      delete afunc;
      return NULL;
      }
    if( !can_assign( (arg_enum) cfunc_list[i].arg[j], (arg_enum) afunc->arg[j]->type ) ) {
      sprintf( error_buf,
        "Passing %s to function %s for argument %d, requires %s.",
        arg_type_name[ afunc->arg[j]->type ],
        cfunc_list[i].name, j+1,
        arg_type_name[ cfunc_list[i].arg[j] ] );
      delete afunc;
      return NULL;
      }
    if( *code == ',' ) 
      code++;
    }

  code++;

  return afunc;
}


/*
 *   STRINGS
 */


arg_type* read_string( const char*& code, extra_array& data )
{
  arg_type*       arg;
  const char*  string;
  char         letter;

  if( *code != '"' && *code != '#' )
    return NULL;

  arg         = new arg_type;
  arg->type   = STRING;
  arg->family = constant;

  string = code+1;

  if( *code == '"' ) {
    for( code++; *code != '"'; code++ ) {
      if( *code == '\0' ) {
        strcpy( error_buf, "Unexpected end of string." );
        delete arg;
        return NULL;
        }
      }
    *((char*)code) = '\0';
    arg->value     = (void*) code_alloc( string );
    *((char*)code) = '"';
    code++;
    return arg;
    }

  for( code++; *code != ',' && !isspace( *code ); code++ ) 
    if( *code == '\0' ) {
      strcpy( error_buf, "Unexpected end of string." );
      delete arg;
      return NULL;
      }

  letter         = *code;
  *((char*)code) = '\0';
  arg->value     = (void*) get_string( string, data );

  if( arg->value == NULL ) {
    sprintf( error_buf, "String not found. (%s)", string ); 
    delete arg;
    arg = NULL;
    } 

  *((char*)code) = letter;

  return arg;
}


/*
 *   NUMBERS
 */


arg_type* itoarg( int i )
{
  arg_type*   arg  = new arg_type;

  arg->type   = INTEGER;
  arg->family = constant;
  arg->value  = (void*) i;

  return arg;
}


arg_type* read_digit( const char*& code )
{
  afunc_type*  afunc;
  int           dice;
  int           plus;
  int           side;
  int              i;
  bool      negative  = FALSE;

  if( *code == '-' ) {
    negative = TRUE;
    code++;
    }

  for( dice = 0; isdigit( *code ); code++ )
    dice = *code+10*dice-'0';

  if( negative )
    dice = -dice;

  if( *code != 'd' )
    return itoarg( dice );

  for( plus = side = 0, code++; isdigit( *code ); code++ )
     side = *code+10*side-'0';

  if( *code == '+' ) 
    for( code++; isdigit( *code ); code++ )
      plus = *code+10*plus-'0';

  afunc         = new afunc_type;
  afunc->type   = INTEGER;
  afunc->family = function;

  afunc->arg[0] = itoarg( dice );
  afunc->arg[1] = itoarg( side );
  afunc->arg[2] = itoarg( plus );

  for( i = 0; ; i++ ) 
    if( !strcasecmp( cfunc_list[i].name, "dice" ) ) {
      afunc->func = &cfunc_list[i];
      break;
      }

  return afunc;
}


















