#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


area_data*  area_list;


const char*  area_status [ MAX_AREA_STATUS ] = {
  "open", "worthless", "abandoned", "progressing", "pending",
  "blank", "immortal" };


/*
 *   AREA_DATA CLASS
 */


Area_Data :: Area_Data( )
{
  record_new( sizeof( area_data ), MEM_AREA );

  room_first   = NULL;
  age          = 15;
  nplayer      = 0;
  modified     = TRUE;
}


/*
 *   DISK ROUTINES
 */


bool set_area_file( char_data* ch, char* arg, const char*& name )
{
  FILE*         fp;
  area_data*  area;
  int            i;

  for( i = 0; arg[i] != '\0'; i++ )
    if( !isalpha( arg[i] ) ) {
      send( ch, "An area file name may only contain letters.\n\r" );
      return FALSE;
      }

  if( i < 3 || i > 12 ) {
    send( ch, "An area file name must be between 3 and 12 letters.\n\r" );
    return FALSE;
    }

  for( area = area_list; area != NULL; area = area->next ) 
    if( !strcasecmp( area->file, arg ) ) {
      send( ch, "There is already an area with that filename.\n\r" );
      return FALSE;
      }

  if( ( fp = open_file( AREA_LIST, "w" ) ) == NULL )
    return FALSE;

  swap( arg, (char*&) name );  

  for( area = area_list; area != NULL; area = area->next ) {
    fprintf( fp, "%s\n", area->file );
    if( area->file == name ) {
      save_area( area );
      }
    }

  fprintf( fp, "$\n\n" );
  fclose( fp );

  swap( arg, (char*&) name );  

  return TRUE;
}


void load_area( const char* file )
{
  char*        tmp  = static_string( );
  area_data*  area;
  FILE*         fp;

  sprintf( tmp, "%s.are", file );
  fp = open_file( AREA_DIR, tmp, "r", TRUE );

  area       = new area_data;
  area->file = alloc_string( file, MEM_AREA );

  if( strcmp( fread_word( fp ), "#AREA" ) ) 
    panic( "Load_area: missing header" );

  area->name      = fread_string( fp, MEM_AREA );
  area->creator   = fread_string( fp, MEM_AREA );
  area->help      = fread_string( fp, MEM_AREA );
 
  area->level       = fread_number( fp );
  area->reset_time  = fread_number( fp );
  area->status      = fread_number( fp );

  fprintf( stdout, "  -%s\n\r", area->name );

  append( area_list, area );
  load_rooms( fp, area );
  fclose( fp );
}


bool save_area( area_data* area, bool forced )
{
  char*            tmp  = static_string( );            
  action_data*  action;
  room_data*      room;
  exit_data*      exit;
  FILE*             fp;

  if( !forced && !area->modified )
    return FALSE;

  sprintf( tmp, "%s.are", area->file );
  if( ( fp = open_file( AREA_DIR, tmp, "w" ) ) == NULL )
    return FALSE;

  fprintf( fp, "#AREA\n" );
  fprintf( fp, "%s~\n", area->name );
  fprintf( fp, "%s~\n", area->creator );
  fprintf( fp, "%s~\n\n", area->help );
  fprintf( fp, "%d %d\n", area->level, area->reset_time );
  fprintf( fp, "%d\n", area->status );
  fprintf( fp, "#ROOMS\n\n" );

  for( room = area->room_first; room != NULL; room = room->next ) {
    fprintf( fp, "#%d\n", room->vnum );
    fprintf( fp, "%s~\n", room->name );
    fprintf( fp, "%s~\n", room->description );
    fprintf( fp, "%s~\n", room->comments ); 
    fprintf( fp, "%d %d %d 0\n", room->room_flags, 
      room->sector_type, room->size );

    for( int i = 0; i < room->exits; i++ ) {
      exit = room->exits[i];
      fprintf( fp, "D%d\n", exit->direction );
      fprintf( fp, "%s~\n", exit->name );
      fprintf( fp, "%s~\n", exit->keywords );
      fprintf( fp, "%d %d %d %d %d %d\n", exit->exit_info,
        exit->key, exit->to_room->vnum,
        exit->strength, exit->light, exit->size );
      }

    write_extras( fp, room->extra_descr );

    for( action = room->action; action != NULL; action = action->next ) 
      write( fp, action ); 

    write( fp, room->reset );
 
    fprintf( fp, "S\n\n" );
    }

  fprintf( fp, "#0\n\n" );
  fclose( fp );

  area->modified = FALSE;

  return TRUE;
}


/*
 *   AREA LIST COMMAND
 */


void room_range( area_data* area, int& low, int& high, int &nRooms )
{
  room_data*  room;

  low    = 99999;
  high   = 0;
  nRooms = 0;

  for( room = area->room_first; room != NULL; room = room->next ) {
    nRooms++;
    low  = min( low, room->vnum );
    high = max( high, room->vnum );
    }

  return;
}


/*
 *   AREA/ROOM SUMMARY COMMANDS
 */

void do_areas( char_data* ch, char* argument )
{
  char         tmp  [ TWO_LINES ];
  area_data*  area;
  int          min;
  int          max;
  int       nRooms;
  int            i;
  int       status;
  int       length  = strlen( argument );

  if( ch->link == NULL )
    return;

  if( *argument == '\0' ) {
    page_title( ch, "Areas" );
    for( i = 0, area = area_list; area != NULL; area = area->next ) {
      if( area->help != empty_string && area->status == AREA_OPEN ) 
        page( ch, "%24s%s", area->name, (i++)%3 != 2 ? "" : "\n\r" );
      }
    if( i%3 != 0 )
      page( ch, "\n\r" );
    page( ch, "\n\r" );
    page_centered( ch, "[ Type area <name> to see more information. ]" );
    return;
    }

  if( is_apprentice( ch ) ) {
    status = -1;
    if( strncasecmp( argument, "summary", length ) )
      for( status = 0; status < MAX_AREA_STATUS; status++ )
        if( !strncasecmp( argument, area_status[status], length ) )
          break;

    if( status != MAX_AREA_STATUS ) {
      sprintf( tmp, "%25s   %11s   %5s  %s\n\r", "Area Name",
        "Vnum Range", "#Plyr", "Creator" );
      page_underlined( ch, tmp );
  
      for( i = 0; i < MAX_AREA_STATUS; i++ ) {
        if( status != -1 && i != status )
          continue;
        *tmp = '\0';
        for( area = area_list; area != NULL; area = area->next ) {
          if( area->status != i )
            continue;
          if( *tmp == '\0' ) {
            page( ch, "\n\r" );
            sprintf( tmp, "--- %s ---", area_status[i] );
            tmp[4] = toupper( tmp[4] );
            page_centered( ch, tmp ); 
            page( ch, "\n\r" );
	    }
          room_range( area, min, max, nRooms );
          page( ch, "%25s  %6d-%-6d  %3d    %s\n\r",
            area->name, min, max, area->nplayer, area->creator );
          }
        }
      return;
      }
    }

  for( area = area_list; area != NULL; area = area->next ) 
    if( area->help != empty_string && area->status == AREA_OPEN
      && !strncasecmp( area->name, argument, length ) )
      break;

  if( area == NULL ) {
    send( ch, "No area matching that name found.\n\r" );
    return;
    }

  page( ch, "         Name: %s\n\r", area->name );
  page( ch, "      Creator: %s\n\r", area->creator );

  if( is_builder( ch ) ) {
    room_range( area, min, max, nRooms );
    page( ch, "        Rooms: %d\n\r", nRooms );
    page( ch, "   Vnum Range: %d to %d\n\r", min, max );
    }

  page( ch, "Approx. Level: %d\n\r\n\r", area->level );

  help_link( ch->link, area->help );

  return;
}


void do_roomlist( char_data* ch, char* )
{
  room_data*  room;

  if( ( room = Room( ch->array->where ) ) == NULL ) {
    send( ch, "You aren't in a room.\n\r" );
    return;
    } 

  page_underlined( ch, "Vnum     Name of Room\n\r" );

  for( room = room->area->room_first; room != NULL; room = room->next ) 
    page( ch, "%-6d   %s\n\r", room->vnum, room->name );
}



