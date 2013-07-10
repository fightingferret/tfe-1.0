#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"
 

/*
 *   LOCAL FUNCTIONS
 */


void   clear_variables        ( void );
void   free_code              ( struct mem_block* );

void    remove_queue  ( queue_data* );  
void*   proc_arg      ( struct arg_type* );

bool end_prog;
bool cont_prog;
bool queue_prog;

queue_data*  queue_list  = NULL;
stack_data*  stack_list  = NULL;


/*
 *   VARIABLES/CONSTANTS
 */


char_data*   var_victim;
char_data*   var_rch;
char_data*   var_mob;  
char_data*   var_ch;
obj_data*    var_obj;
obj_data*    var_container;
room_data*   var_room;
char*        var_arg;
int          var_i;
int          var_j;


/*
 *   MEMORY ROUTINES
 */


char* code_alloc( const char* argument )
{
  mem_block*  block;
  int          size  = strlen( argument );

  if( size == 0 )
    return empty_string;

  block       = new mem_block( size+1 );
  block->next = block_list;
  block_list  = block;

  memcpy( block->pntr, argument, size+1 );

  return (char*) block->pntr;
}


/*
 *   BUG ROUTINE
 */


void code_bug( char *text )
{
  char tmp [ MAX_INPUT_LENGTH ];

  bug( text );

  sprintf( tmp, "-- Char = %s  Mob = %s  Victim = %s",
    var_ch  == NULL    ? "NULL" : var_ch->descr->name,
    var_mob == NULL    ? "NULL" : var_mob->descr->name,
    var_victim == NULL ? "NULL" : var_victim->descr->name );
  bug( tmp );
  
  sprintf( tmp, "-- Rch = %s, Room #%d, i = %d",
    var_rch == NULL  ? "NULL" : var_rch->descr->name,
    var_room == NULL ? -1 : var_room->vnum, var_i );  
  bug( tmp );
}


/*
 *   STACK ROUTINES
 */


void push( )
{
  stack_data* stack;

  stack = new stack_data;
  
  stack->room   = var_room;
  stack->ch     = var_ch;
  stack->victim = var_victim;
  stack->obj    = var_obj;
  stack->mob    = var_mob;
  stack->rch    = var_rch;
  stack->i      = var_i;

  stack->next = stack_list;
  stack_list  = stack;
}


void pop( )
{
  stack_data* stack;

  stack      = stack_list;
  stack_list = stack->next;

  var_room   = stack->room;
  var_ch     = stack->ch;
  var_victim = stack->victim;
  var_obj    = stack->obj;
  var_mob    = stack->mob;
  var_rch    = stack->rch;
  var_i      = stack->i;

  delete stack;
}


/*
 *   QUEUE ROUTINES
 */


void do_ps( char_data* ch, char* )
{
  queue_data* queue;

  if( queue_list == NULL ) {
    send( ch, "The queue is empty.\n\r" );
    return;
    }

  page_underlined( ch, "Vnum     Type        Location\n\r" );   

  for( queue = queue_list; queue != NULL; queue = queue->next )
     queue->program->display( ch );
}  


void update_queue( void )
{
  struct queue_data *queue, *queue_next;
  struct mem_block *tmp_list = block_list;

  for( queue = queue_list; queue != NULL; queue = queue_next ) {
    queue_next = queue->next;

    if( queue->time-- > 1 )
      continue;

    var_room = queue->room;
    var_ch   = queue->ch;
    var_mob  = queue->mob;
    var_obj  = queue->obj;
    var_i    = queue->i;

    error_buf[0] = '\0';
    end_prog     = FALSE;
    cont_prog    = FALSE;
    queue_prog   = FALSE;

    proc_arg( queue->arg->next );

    if( queue_prog ) 
      queue_list->program = queue->program;
    else 
      queue->program->active--;
         
    remove( queue_list, queue );
    delete queue;
    }

  clear_variables( );
}


/*
 *   CLEAR QUEUE OF A VARIABLE
 */


void clear_queue( char_data* ch )
{
  queue_data* queue;
  stack_data* stack;

  for( queue = queue_list; queue != NULL; queue = queue->next ) {
    if( queue->mob == ch )  queue->mob = NULL;
    if(  queue->ch == ch )   queue->ch = NULL;
    }

  if(    var_mob == ch )      var_mob = NULL;
  if(     var_ch == ch )       var_ch = NULL;
  if( var_victim == ch )   var_victim = NULL;
  if(    var_rch == ch )      var_rch = NULL;

  for( stack = stack_list; stack != NULL; stack = stack->next ) {
    if(     stack->ch == ch )      stack->ch = NULL;
    if( stack->victim == ch )  stack->victim = NULL;
    if(    stack->rch == ch )     stack->rch = NULL;
    if(    stack->mob == ch )     stack->mob = NULL;
    } 
}


void clear_queue( obj_data* obj )
{
  queue_data* queue;
  stack_data* stack;

  for( queue = queue_list; queue != NULL; queue = queue->next ) 
    if( queue->obj == obj )
      queue->obj = NULL;

  if(       var_obj == obj )         var_obj = NULL;
  if( var_container == obj )   var_container = NULL;

  for( stack = stack_list; stack != NULL; stack = stack->next ) 
    if( stack->obj == obj )
      stack->obj = NULL;
}


void clear_queue( program_data* program )
{
  queue_data*  queue;
  queue_data*   next;

  for( queue = queue_list; queue != NULL; queue = next ) {
    next = queue->next; 
    if( queue->program == program )  
      remove( queue_list, queue );
    }

  program->active = 0;
}


/*
 *   RUN-TIME FUNCTIONS 
 */


bool execute( program_data* program )
{
  bool flag;

  if( program->binary == NULL ) {
    compile( program );
    if( program->binary == NULL ) {
      delete_list( program->memory );
      return FALSE;
      }
    }

  if( program->active > 50 ) {
    bug( "Execute: infinite recursive loop." );
    return FALSE;
    }

  end_prog   = FALSE;
  cont_prog  = FALSE;
  queue_prog = FALSE;

  program->active++;
  proc_arg( program->binary );

  if( queue_prog ) 
    queue_list->program = program;
  else 
    program->active--;

  flag = cont_prog;

  end_prog   = FALSE;
  cont_prog  = FALSE;
  queue_prog = FALSE;

  clear_variables( );

  return flag;
}


void* proc_arg( struct arg_type* arg )  
{
  afunc_type*    afunc;
  arg_type*   arg_next;
  void*           farg  [4];
  void**          pntr;
  void*         result;
  int                i;

  if( end_prog )
    return NULL;  

  if( arg == NULL )
    return NULL;

  arg_next = arg->next;

  if( arg_next != NULL ) {
    arg->next = NULL;
    proc_arg( arg );
    arg->next = arg_next;
    if( end_prog )
      return NULL;
    proc_arg( arg_next );
    return NULL;
    }

  if( arg->family == cont ) {
    cont_prog = TRUE;
    end_prog  = TRUE;
    return NULL;
    }

  if( arg->family == end ) {
    end_prog = TRUE;
    return NULL;
    }

  if( arg->family == constant ) 
    return arg->value;

  if( arg->family == variable ) {
    pntr = (void**) arg->value;
    return( arg->neg ? *pntr == NULL : *pntr );   
    }

  if( arg->family == loop ) {
    loop_type*      aloop  = (loop_type*) arg;
    char_data*        rch;
    char_data*     leader  = var_ch;
   
    if( aloop->fruit == loop_unknown ) {
      for( i = 0; i < 100; i++ ) {
        if( ( proc_arg( aloop->condition ) == NULL ) != aloop->neg )
          break;
        proc_arg( aloop->aloop );
        }
      if( i == 100 )
        bug( "Proc_arg: Infinite script loop." );
      return NULL;
      }    

    if( var_room == NULL ) {
      code_bug( "Proc_arg: NULL room in loop_followers." );
      return NULL;
      }

    for( i = 0; i < var_room->contents; i++ ) {
      if( ( rch = character( var_room->contents[i] ) ) == NULL
        || ( aloop->fruit == loop_followers && rch->leader != leader ) )
        continue; 
      var_rch = rch;
      proc_arg( aloop->aloop );
      }       

    return NULL;
    }

  if( arg->family == if_clause ) {
    aif_type*   aif  = (aif_type*) arg; 
    void*      flag  = proc_arg( aif->condition );

    if( flag == NULL )
      proc_arg( aif->no );
    else
      proc_arg( aif->yes );

    return NULL;
    }

  afunc = (afunc_type*) arg;

  for( i = 0; i < 4; i++ )
    farg[i] = NULL; 

  if( afunc->func->func_call == code_set_equal ) {
    farg[0] = afunc->arg[0];         
    farg[1] = proc_arg( afunc->arg[1] );
    code_set_equal( farg );
    return NULL;
    }

  for( i = 0; i < 4; i++ ) {
    if( afunc->arg[i] == NULL )
      break; 
    farg[i] = proc_arg( afunc->arg[i] );         
    }

  curr_arg = arg;
  result   = ( afunc->func->func_call )( farg );

  if( !arg->neg )
    return result;

  return (void*) ( result == NULL );
}


/*
 *   MISC SUPPORT ROUTINES
 */


char* get_string( const char* name, extra_array& list )
{
  for( int i = 0; i < list; i++ )
    if( !strcasecmp( name, list[i]->keyword ) )
      return list[i]->text;

  return NULL;
}


void clear_variables( )
{
  var_victim    = NULL;
  var_rch       = NULL;
  var_mob       = NULL;
  var_ch        = NULL;
  var_obj       = NULL;
  var_container = NULL;
  var_room      = NULL;
} 



  

