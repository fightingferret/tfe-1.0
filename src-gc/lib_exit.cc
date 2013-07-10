#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"


void* code_open( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  int         door  = (int)        argument[1];
  exit_data*  exit;

  if( room != NULL
    && ( exit = exit_direction( room, door ) ) != NULL ) {
    remove_bit( &exit->exit_info, EX_CLOSED );
    if( ( exit = reverse( exit ) ) != NULL )
      remove_bit( &exit->exit_info, EX_CLOSED );
    }

  return NULL;
}


void* code_close( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  int         door  = (int)        argument[1];
  exit_data*  exit;

  if( room != NULL
    && ( exit = exit_direction( room, door ) ) != NULL ) {
    set_bit( &exit->exit_info, EX_CLOSED );
    if( ( exit = reverse( exit ) ) != NULL )
      set_bit( &exit->exit_info, EX_CLOSED );
    }

  return NULL;
}


void* code_lock( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  int         door  = (int)        argument[1];
  exit_data*  exit;

  if( room != NULL
    && ( exit = exit_direction( room, door ) ) != NULL ) {
    set_bit( &exit->exit_info, EX_LOCKED );
    if( ( exit = reverse( exit ) ) != NULL )
      set_bit( &exit->exit_info, EX_LOCKED );
    }

  return NULL;
}


void* code_unlock( void** argument )
{
  room_data*   room  = (room_data*) argument[0];
  int          door  = (int)        argument[1];
  exit_data*   exit;

  if( room != NULL
    && ( exit = exit_direction( room, door ) ) != NULL ) {
    remove_bit( &exit->exit_info, EX_LOCKED );
    if( ( exit = reverse( exit ) ) != NULL )
      remove_bit( &exit->exit_info, EX_LOCKED );
    }

  return NULL;
}


void* code_is_open( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  int          dir  = (int)        argument[1];
  exit_data*  exit;  

  return (void*) ( room != NULL
    && ( exit = exit_direction( room, dir ) ) != NULL
    && !is_set( &exit->exit_info, EX_CLOSED ) );
}


void* code_is_locked( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  int          dir  = (int)        argument[1];
  exit_data*  exit;  

  return (void*) ( room != NULL
    && ( exit = exit_direction( room, dir ) ) != NULL
    && is_set( &exit->exit_info, EX_LOCKED ) );
}


void* code_show( void** argument )
{
  char_data*    ch  = (char_data*) argument[0];
  room_data*  room  = (room_data*) argument[1];
  int          dir  = (int)        argument[2];
  exit_data*  exit;  

  if( room != NULL && ch != NULL
    && ( exit = exit_direction( room, dir ) ) != NULL     
    && is_set( &exit->exit_info, EX_SECRET ) )
    ch->seen_exits += exit;

  return NULL;
}  



