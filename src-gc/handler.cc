#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


/*
 *   LIST GENERATING FUNCTIONS
 */


int get_trust( char_data *ch )
{
  if( ch->pcdata == NULL )
    return LEVEL_HERO - 1;

  if( ch->pcdata->trust != 0 )
    return ch->pcdata->trust;

  return ch->shdata->level;
}


/*
 *   WEIRD ROUTINES
 */


char_data* rand_player( room_data* room )
{
/*
  char_data*  ch;
  int          j;

  for( j = 0, ch = room->people; ch != NULL; j += !IS_NPC( ch ),
    ch = ch->next_in_room );
  
  if( j == 0 )
    return NULL;

  j = number_range( 1, j );

  for( ch = room->people; ch != NULL; ch = ch->next_in_room )    
    if( ( j -= !IS_NPC( ch ) ) == 0 )
      break;

  return ch;
*/
  return NULL;
} 


char_data* rand_victim( char_data* )
{
  /*
  char_data*  rch;
  int           i = 0;

  for( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    if( rch != ch && ( rch->Seen( ch ) || rch->fighting == ch ) )
      i++;

  if( i == 0 )
    return NULL;

  for( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    if( rch != ch && ( rch->Seen( ch ) || rch->fighting == ch )
      && --i == 0 )
      break;

  return rch;
  */
  return NULL;
} 
 


