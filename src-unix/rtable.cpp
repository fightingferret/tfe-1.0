#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"


int              num_rtable  = 0;
rtable_data**   rtable_list  = NULL;


void  renumber_rtable   ( char_data*, int, int );
bool  can_extract       ( int, char_data* );
bool  has_reset         ( int );
 

/*
 *   RTABLE_DATA NEW AND DELETE
 */


Rtable_Data :: Rtable_Data( )
{
  name  = empty_string;
  reset = NULL;
 
  return;
}


Rtable_Data :: ~Rtable_Data( )
{
  return;
}


/*
 *   SUPPORT ROUTINES
 */


bool can_extract( int index, char_data* ch )
{
  if( has_reset( index ) ) {
    send( ch, "That table still has resets and cannot be extracted.\n\r" );
    return FALSE;
    }

  return TRUE;
}


bool has_reset( int index )
{
  reset_data*      reset;
  species_data*  species;
  area_data*        area;
  room_data*        room;
  int                  i;

  for( i = 0; i < MAX_SPECIES; i++ ) 
    if( ( species = species_list[i] ) != NULL ) 
      for( reset = species->reset; reset != NULL; reset = reset->next ) 
        if( index == reset->vnum 
          && !is_set( &reset->flags, RSFLAG_MOB )
          && !is_set( &reset->flags, RSFLAG_OBJECT ) ) 
          return TRUE;

  for( area = area_list; area != NULL; area = area->next )
    for( room = area->room_first; room != NULL; room = room->next ) 
      for( reset = room->reset; reset != NULL; reset = reset->next ) 
        if( index == reset->vnum 
          && !is_set( &reset->flags, RSFLAG_MOB )
          && !is_set( &reset->flags, RSFLAG_OBJECT ) ) 
          return TRUE;

  for( i = 0; i < num_rtable; i++ ) 
    for( reset = rtable_list[i]->reset; reset != NULL; reset = reset->next )
      if( index == reset->vnum 
        && !is_set( &reset->flags, RSFLAG_MOB )
        && !is_set( &reset->flags, RSFLAG_OBJECT ) ) 
        return TRUE;
  
  return FALSE;
}


/*
 *  EDITING ROUTINE
 */


void do_rtable( char_data* ch, char* argument )
{
  rtable_data*   rtable;
  wizard_data*   wizard  = (wizard_data*) ch;
  int                 i  = wizard->rtable_edit;
  int                 j;
  bool             flag;

  if( matches( argument, "exit" ) || !strcmp( argument, ".." ) ) {
    if( i == - 1 )
      send( ch, "You already aren't editting a rtable.\n\r" );
    else {
      send( ch, "You stop editting rtable '%s'.\n\r",
        rtable_list[i]->name );
      wizard->rtable_edit = -1;
      }
    return;
    }

  if( i != -1 ) {
    edit_reset( ch, argument, rtable_list[i]->reset, RST_TABLE,
      rtable_list[i]->name );
    return;
    }
    
  if( *argument == '\0' ) {
    page_title( ch, "Reset Tables" );
    for( i = 0; i < num_rtable; i++ ) 
      send( ch, "[%2i] %s\n\r", i+1, rtable_list[i]->name );
    return;
    }

  if( exact_match( argument, "new" ) ) { 
    if( *argument == '\0' ) {
      send( ch, "Name of new table?\n\r" );
      return;
      }
    rtable        = new rtable_data;
    rtable->name  = alloc_string( argument, MEM_RESET );
    insert( rtable_list, num_rtable, rtable, num_rtable );
    send( ch, "Rtable '%s' created.\n\r", argument );
    return;
    }

  flag = matches( argument, "delete" );

  if( number_arg( argument, i ) ) {
    if( --i < 0 || i >= num_rtable ) { 
      send( ch, "No rtable exists with that index.\n\r" );
      return;
      }
    rtable = rtable_list[i];
    if( flag ) {
      if( can_extract( i, ch ) ) {
        send( ch, "Rtable %d, %s removed.\n\r", i+1, rtable->name );
        remove( rtable_list, num_rtable, i );
        delete rtable;
        }
      return;
      } 
    if( *argument == '\0' ) {
      wizard->rtable_edit = i;
      send( ch, "You are now editing rtable %d, '%s'.\n\r",
        i+1, rtable->name );
      }
    else if( number_arg( argument, j ) ) {
      renumber_rtable( ch, i, j );
      }
    else {
      send( ch, "Rtable %d, '%s' renamed '%s'.\n\r", 
        i+1, rtable->name, argument );
      free_string( rtable->name, MEM_RESET );
      rtable->name = alloc_string( argument, MEM_RESET );
      }
    return;
    }

  send( ch, "Illegal syntax - See help rtable.\n\r" );
}


void renumber_rtable( char_data* ch, int i, int j )
{
  area_data*         area;
  room_data*         room;
  reset_data*       reset;
  species_data*   species;
  int                   k;
  rtable_data*     rtable  = rtable_list[i];

  if( --j < 0 || j >= num_rtable ) {
    send( ch, "You can only move a rtable to a sensible position.\n\r" );
    return;
    }

  if( i == j ) {
    send( ch, "Moving a rtable to where it already is does nothing\
 interesting.\n\r" );
    return;
    }

  remove( rtable_list, num_rtable, i );
  insert( rtable_list, num_rtable, rtable, j );

  send( ch, "Rtable '%s' moved to position %d.\n\r",
    rtable->name, j+1 );

  /* RENUMBER RESETS */

  for( area = area_list; area != NULL; area = area->next ) 
    for( room = area->room_first; room != NULL; room = room->next ) 
      for( reset = room->reset; reset != NULL; reset = reset->next ) 
        if( !is_set( &reset->flags, RSFLAG_OBJECT )
          && !is_set( &reset->flags, RSFLAG_MOB )  
          && renumber( reset->vnum, i, j ) )
          area->modified = TRUE;

  for( k = 0; k < MAX_SPECIES; k++ ) 
    if( ( species = species_list[k] ) != NULL ) 
      for( reset = species->reset; reset != NULL; reset = reset->next ) 
        if( !is_set( &reset->flags, RSFLAG_OBJECT )
          && !is_set( &reset->flags, RSFLAG_MOB ) )
          renumber( reset->vnum, i, j );

  return;
}


/*
 *   DISK ROUTINES
 */


void load_rtables( )
{
  FILE*              fp;
  int              i, j;
  reset_data*     reset;  
  rtable_data*   rtable;

  fprintf( stderr, "Loading Reset Tables...\n\r" );
 
  fp = open_file( RTABLE_FILE, "r" );

  num_rtable  = fread_number( fp );
  rtable_list = new rtable_data*[num_rtable];

  for( i = 0; i < num_rtable; i++ ) {
    rtable         = new rtable_data;
    rtable->name   = fread_string( fp, MEM_RESET );
    rtable_list[i] = rtable;
    for( ; ; ) {
      if( ( j = fread_number( fp ) ) == -1 )
        break;
      reset          = new reset_data;
      reset->vnum    = j;
      reset->flags   = fread_number( fp );
      reset->chances = fread_number( fp );
      reset->value   = fread_number( fp );
      append( rtable->reset, reset );
      }
    }

  fclose( fp );
}


void save_rtables( )
{
  FILE*            fp;
  reset_data*   reset;

  if( ( fp = open_file( RTABLE_FILE, "w" ) ) == NULL ) 
    return;

  fprintf( fp, "%d\n\n", num_rtable );

  for( int i = 0; i < num_rtable; i++ ) {
    fprintf( fp, "%s~\n", rtable_list[i]->name );
    for( reset = rtable_list[i]->reset; reset != NULL;
      reset = reset->next ) {
      fprintf( fp, "%d %d %d %d\n", reset->vnum, reset->flags,
        reset->chances, reset->value );
      }
    fprintf( fp, "-1\n\n" );
    }

  fclose( fp );
}


/*
 *   RTWHERE
 */


void do_rtwhere( char_data* ch, char* argument )
{
  area_data*           area;
  reset_data*         reset;
  room_data*           room;
  species_data*     species;
  bool                found;
  int                 index;
  int                     i;

  if( *argument == '\0' ) {
    send( ch, "Syntax: rtwhere <table>\n\r" );
    return;
    }

  index = atoi( argument )-1;
  found = FALSE;

  if( index < 0 || index >= num_rtable ) {
    send( ch, "That isn't an acceptable rtable index.\n\r" );
    return;
    }

  /* SEARCH MRESETS */

  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL ) 
      continue;
    for( reset = species->reset; reset != NULL; reset = reset->next ) {
      if( index == reset->vnum 
        && !is_set( &reset->flags, RSFLAG_OBJECT )
        && !is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        page( ch, "  On %d, %s\n\r", i, species->descr->name );
        found = TRUE;
	}
      }
    }

  /* SEARCH ROOM RESETS */

  for( area = area_list; area != NULL; area = area->next ) {
    for( room = area->room_first; room != NULL; room = room->next ) {
      species =  NULL;
      for( reset = room->reset; reset != NULL; reset = reset->next ) {
        if( is_set( &reset->flags, RSFLAG_MOB ) ) {
          species = get_species( reset->vnum );
          continue;
          }
        if( index != reset->vnum 
          || is_set( &reset->flags, RSFLAG_OBJECT ) )
          continue;
        found = TRUE;
        if( reset->value == -2 ) {
          page( ch, "  At room %d, %s\n\r", room->vnum, room->name );
          } 
        else if( species == NULL ) {
          page( ch, "  [BUG] Illegal reset structure [%d]\n\r",
            room->vnum );
          }
        else {
          page( ch, "  On %s at %s [%d]\n\r",
            species->descr->name, room->name, room->vnum );
          }
        }
      }
    }

  /* SEARCH TABLES */

  for( i = 0; i < num_rtable; i++ ) 
    for( reset = rtable_list[i]->reset; reset != NULL; reset = reset->next )
      if( reset->vnum == index 
        && !is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        page( ch, "  In rtable %d, %s.\n\r", 
          i+1, rtable_list[i]->name );
        found = TRUE;
        }

  if( !found )
    page( ch, "  no resets found\n\r" ); 

  return;
}
