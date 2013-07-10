#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   BUG, IDEA, TYPO FILE ROUTINES
 */


void panic( const char* text )
{
  bug( text );
  bug( "** MUD KILLED **" );

  exit( 1 );
}


void bug( int level, const char* str )
{
  FILE*    fp;

  if( str == NULL || str == empty_string )
    return;

  if( level != BUG_APHID ) {
    if( ( fp = fopen( BUG_FILE, "a" ) ) != NULL ) {
      fprintf( fp, "[%s] %s\n", ltime( current_time )+4, str );
      fclose( fp );
      }
    else {
      info( "", LEVEL_BUILDER, "Error opening bug file.", IFLAG_BUGS, 1 );
      fprintf( stderr, "[BUG] Error opening bug file.\n" );
      }
    }

  if( level != -1 ) {
    fprintf( stderr, "[BUG] %s\n\r", str );
    info( "", LEVEL_BUILDER, str, IFLAG_BUGS, level );
    }

  return;
}


/*
 *   DEFINE COMMAND
 */


const char* lookup( index_data* index, int number, bool plural )
{
  int i;

  for( i = 0; index[i].value < number && index[i].value != -1; i++ );

  return( plural ? index[i].plural : index[i].singular );
}


void do_define( char_data* ch, char* argument )
{
  char* name [] = { "Acid Damage", "Fire Damage", "Cold Damage",
    "Electrical Damage", "Physical Damage", "Fame", "Piety",
    "Reputation", "" };

  index_data* index [] = { acid_index, fire_index, cold_index,
    electric_index, physical_index, fame_index, piety_index,
    reputation_index };

  int           prev;
  int           i, j;

  if( *argument == '\0' ) {
    send_title( ch, "Defined Terms" );
    for( i = 0; *name[i] != '\0'; i++ ) 
      send( ch, "%20s%s", name[i], i%3 == 2 || *name[i+1] == '\0'
        ? "\n\r" : "" );
    return;
    }

  for( i = 0; *name[i] != '\0'; i++ ) {
    if( fmatches( argument, name[i] ) ) {
      page_title( ch, name[i] );
      prev = ( index[i][0].value < 0 ? -1000 : 0 );
      for( j = 0; ; j++ ) {
        if( j == 0 && index[i][0].value == 0 )
          continue;
        if( index[i][j].value == -1 ) {
          page( ch, "%39s   %d+\n\r", index[i][j].singular, prev );
          return;
          }
        if( index[i][j].value == prev )
          page( ch, "%39s   %d\n\r", index[i][j].singular, prev );
        else
          page( ch, "%39s   %d to %d\n\r",
            index[i][j].singular, prev, index[i][j].value );
        prev = index[i][j].value+1;
        }
      }
    }

  send( ch, "Unknown field - see help define.\n\r" );
}


/*
 *   TYPO COMMAND
 */


void do_typo( char_data* ch, char* argument )
{
  char tmp [ MAX_STRING_LENGTH ];

  if( *argument == '\0' ) {
    send( ch, "Room #%d\n\r\n\r", ch->in_room->vnum );
    send( ch, "What typo do you wish to report?\n\r" );
    return;
    } 

  if( strlen( ch->in_room->comments )
    > MAX_STRING_LENGTH-MAX_INPUT_LENGTH-30 ) {
    send( ch, "Comment field at max length - typo ignored.\n\r" );
    return;
    }

  ch->in_room->area->modified = TRUE;

  sprintf( tmp, "%s[%s] %s\n\r", ch->in_room->comments, 
    ch->real_name( ), argument );

  free_string( ch->in_room->comments, MEM_ROOM );
  ch->in_room->comments = alloc_string( tmp, MEM_ROOM );

  send( "Typo noted - thanks.\n\r", ch );

  return;
}    


/*
 *   RANDOM SUPPORT ROUTINES
 */


bool player_in_room( room_data* room )
{
  char_data* ch;

  if( room != NULL )
    for( int i = 0; i < room->contents; i++ )
      if( ( ch = character( room->contents[i] ) ) != NULL 
        && ch->pcdata != NULL )
        return TRUE;

  return FALSE;
};










