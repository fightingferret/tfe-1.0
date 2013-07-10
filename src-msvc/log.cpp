#include "define.h"
#include "struct.h"


bool  view_file    ( char_data*, char* );
void  add_log      ( const char*, const char*, char_data*, const char* );


/*
 *   SUPPORT FUNCTIONS
 */


void add_log( const char* function, const char* file, char_data* ch,
  const char* string )
{
  char      tmp  [ MAX_INPUT_LENGTH+20 ];
  FILE*      fp;

  if( ( fp = fopen( file, "a" ) ) == NULL ) {
    bug( "%s: Could not open file.", function );
    return;
    }

  sprintf( tmp, "[%s] %s", ltime( current_time )+4, string );

  tmp[15] = toupper( tmp[15] );

  if( strlen( tmp ) > 60 ) {
    sprintf( tmp+60, "...  %s\r\n", ch->real_name( ) );
    fprintf( fp, tmp );
    }
  else
    fprintf( fp, "%-65s%s\r\n", tmp, ch->real_name( ) );
   
  fclose( fp );

  return;
}


bool view_file( char_data* ch, char* file )
{ 
  FILE*      fp;
  char      tmp  [ MAX_INPUT_LENGTH ];
  int    length;

  if( ( fp = fopen( file, "r" ) ) == NULL ) 
    return FALSE;

  for( length = 0; ; ) {
    tmp[ length ] = getc( fp );
    if( tmp[ length++ ] == EOF ) {
      tmp[ length ] = '\0';
      page( ch, tmp );
      page( ch, "\r\n" );
      break;
      }    
    if( length == MAX_INPUT_LENGTH-1 ) {
      tmp[ MAX_INPUT_LENGTH-1 ] = '\0'; 
      page( ch, tmp );
      length = 0;
      }
    }

  fclose( fp );

  return TRUE;
}


/*
 *   MOB LOG ROUTINES
 */


void mob_log(  char_data* ch, int i, const char* string )
{
  char  file  [ ONE_LINE ];

  if( string != empty_string ) {
    sprintf( file, "%smob.%d", MOB_LOG_DIR, i );
    add_log( "Mob_Log", file, ch, string );
    }

  return;
}


void do_mlog( char_data* ch, char* argument )
{
  char               tmp  [ ONE_LINE ];
  pfile_data*      pfile;
  char_data*      victim  = NULL;
  species_data*  species;

  if( is_number( argument ) ) { 
    if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
      send( ch, "There is no mob with that vnum.\r\n" );
      return;
      }
    }
  else {
    if( ( pfile = find_pfile_exact( argument ) ) == NULL ) {
      if( ( victim = one_character( ch, argument, "mlog",
        (thing_array*) &player_list, (thing_array*) &mob_list ) ) == NULL ) 
        return;
      if( victim->pcdata != NULL )
        pfile = victim->pcdata->pfile;
      }
    if( pfile != NULL ) {
      sprintf( tmp, "%s%s", PLAYER_LOG_DIR, pfile->name );
      if( !view_file( ch, tmp ) )
        send( ch, "%s has no log.\r\n", pfile->name );
      return;
      }
    species = victim->species;
    } 

  sprintf( tmp, "%smob.%d", MOB_LOG_DIR, species->vnum );
  if( !view_file( ch, tmp ) )
    send( ch, "%s has no log.\r\n", species->Name( ) );
 
  return;
}


/*
 *   OBJECT LOG FILES
 */


void obj_log( char_data* ch, int i, const char* string )
{
  char   file  [ ONE_LINE ];

  if( string != empty_string ) {
    sprintf( file, "%sobj.%d", OBJ_LOG_DIR, i );
    add_log( "Obj_Log", file, ch, string );
    }

  return;
}


void do_olog( char_data* ch, char* argument )
{
  char*                tmp  = static_string( );
  wizard_data*      wizard  = (wizard_data*) ch;
  obj_clss_data*  obj_clss;
  int                    i;

  if( *argument == '\0' ) {
    if( ( obj_clss = wizard->obj_edit ) == NULL ) {
      send( ch,
        "You are not editting an object so must specify a vnum.\r\n" );
      return;
      }
    i = obj_clss->vnum;
    }
  else {
    if( get_obj_index( i = atoi( argument ) ) == NULL ) {
      send( ch, "There is no object with that vnum.\r\n" );
      return;
      }
    }

  sprintf( tmp, "%sobj.%d", OBJ_LOG_DIR, i );
  if( !view_file( ch, tmp ) )
    send( ch, "That object has no log.\r\n" );
 
  return;
}


/*
 *   ROOM LOG FUNCTIONS
 */


void room_log(  char_data* ch, int i, const char* string )
{
  char file  [ ONE_LINE ];

  if( string != empty_string ) {
    sprintf( file, "%sroom.%d", ROOM_LOG_DIR, i );
    add_log( "Room_Log", file, ch, string );
    }

  return;
}


void do_rlog( char_data* ch, char* )
{
  char tmp [ ONE_LINE ];

  sprintf( tmp, "%sroom.%d", ROOM_LOG_DIR, ch->in_room->vnum );

  if( !view_file( ch, tmp ) )
    send( "There is no log for this room.\r\n", ch );
 
  return;
}


/*
 *   PLAYER LOG FILES
 */


void player_log( char_data* ch, const char* string )
{
  char    file  [ ONE_LINE ];
  FILE*     fp;

  if( ch->pcdata == NULL
    || string == empty_string )
    return;

  sprintf( file, "%s%s", PLAYER_LOG_DIR, ch->real_name( ) );

  if( ( fp = open_file( file, "a" ) ) == NULL )
    return;

  fprintf( fp, "[%s] %s\r\n", ltime( current_time )+4, string );
  fclose( fp );

  return;
}


/*
 *   GENERAL LOG FILES
 */


void immortal_log( char_data* ch, char* cmd, char* arg )
{
  char    file  [ ONE_LINE ];
  FILE*     fp;
  
  sprintf( file, "%s%s", IMMORTAL_LOG_DIR, ch->real_name( ) );

  if( ( fp = fopen( file, "a" ) ) != NULL ) {
    fprintf( fp, "[%s] %s %s\n",
      ltime( current_time )+4, cmd, arg );
    fclose( fp );
    }

  return;
}



